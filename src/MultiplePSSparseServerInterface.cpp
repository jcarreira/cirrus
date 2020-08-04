#include "MultiplePSSparseServerInterface.h"
#include <cassert>
#include "Constants.h"
#include "PSSparseServerInterface.h"

#undef DEBUG

#define MAX_MSG_SIZE (1024 * 1024)

namespace cirrus {

MultiplePSSparseServerInterface::MultiplePSSparseServerInterface(
    const Configuration& config,
    const std::vector<std::string>& param_ips,
    const std::vector<uint64_t>& ps_ports) {
  std::cout << "Starting Multiple PS " << param_ips.size() << std::endl;
  for (int i = 0; i < param_ips.size(); i++) {  // replace 2 with num_servers
    auto ptr = new PSSparseServerInterface(param_ips[i], ps_ports[i]);
    psints.push_back(ptr);
  }
  minibatch_size = config.get_minibatch_size();
}

void MultiplePSSparseServerInterface::connect() {
  for (auto ptr : psints) {
    std::cout << "Attempting connection to " << ptr->ip << ":" << ptr->port
              << std::endl;
    repeat(std::bind(&PSSparseServerInterface::connect, ptr));
    std::cout << "Connected!!!" << std::endl;
  }
}

void MultiplePSSparseServerInterface::send_lr_gradient(
    const LRSparseGradient& gradient) {
  int num_ps = psints.size();
  uint32_t operation = SEND_LR_GRADIENT;
#ifdef DEBUG
  std::cout << "Sending gradient" << std::endl;
#endif
  int ret;
  for (auto psint : psints) {
    ret = psint->send_all_wrapper(&operation, sizeof(uint32_t));
    if (ret == -1)
      throw std::runtime_error("Error sending operation");
  }

  uint32_t size = gradient.getShardSerializedSize(num_ps);
  char data[size];
  auto starts_and_size = gradient.shard_serialize(data, num_ps);

  for (int i = 0; i < num_ps; i++) {
    auto psint = psints[i];
    auto sas = starts_and_size[i];

    ret = psint->send_all_wrapper(&std::get<1>(sas), sizeof(uint32_t));
    if (ret == -1) {
      throw std::runtime_error("Error sending grad size");
    }
    ret = psint->send_all_wrapper(data + std::get<0>(sas), std::get<1>(sas));
    if (ret == 0) {
      throw std::runtime_error("Error sending grad");
    }
  }
}

void MultiplePSSparseServerInterface::send_mf_gradient(
    const MFSparseGradient& gradient) {
  int num_ps = psints.size();
  uint32_t operation = SEND_MF_GRADIENT;
  int ret;
  // Send the operation to all PS servers
  for (auto psint : psints) {
    ret = psint->send_all_wrapper(&operation, sizeof(uint32_t));
    if (ret == -1)
      throw std::runtime_error("Error sending operation");
  }

  uint32_t size = gradient.getShardSerializedSize(num_ps);
  char data[size];
  auto starts_and_size = gradient.shard_serialize(data, minibatch_size, num_ps);

  for (int i = 0; i < num_ps; i++) {
    auto psint = psints[i];
    auto sas = starts_and_size[i];

    ret = psint->send_all_wrapper(&std::get<1>(sas), sizeof(uint32_t));
    if (ret == -1) {
      throw std::runtime_error("Error sending grad size");
    }
    ret = psint->send_all_wrapper(data + std::get<0>(sas), std::get<1>(sas));
    if (ret == 0) {
      throw std::runtime_error("Error sending grad");
    }
  }
}

SparseLRModel MultiplePSSparseServerInterface::get_lr_sparse_model(
    const SparseDataset& ds,
    const Configuration& config) {
  SparseLRModel model(0);
  get_lr_sparse_model_inplace(ds, model, config);
  return std::move(model);
}

void MultiplePSSparseServerInterface::get_lr_sparse_model_inplace(
    const SparseDataset& ds,
    SparseLRModel& model,
    const Configuration& config) {
  // Initialize variables

  int num_servers = psints.size();

  std::unique_ptr<char*> msg_lst_ptr(new char*[num_servers]);
  char** msg_lst = msg_lst_ptr.get();

  std::unique_ptr<char*> msg_begin_lst_ptr(new char*[num_servers]);
  char** msg_begin_lst = msg_begin_lst_ptr.get();

  std::unique_ptr<uint32_t[]> num_weights_lst(new uint32_t[num_servers]);

  std::array<std::unique_ptr<char>, MAX_NUM_PS> msg_lst_i_ptr;
  for (int i = 0; i < num_servers; i++) {
    msg_lst_i_ptr[i] = std::unique_ptr<char>(new char[MAX_MSG_SIZE]);
    msg_lst[i] = msg_lst_i_ptr[i].get();
    msg_begin_lst[i] = msg_lst_i_ptr[i].get();
    num_weights_lst[i] = 0;
    store_value<uint32_t>(
        msg_lst[i],
        num_weights_lst[i]);  // just make space for the number of weights
  }

  // Split the dataset based on which server data belongs to.
  // XXX consider optimizing this
  for (const auto& sample : ds.data_) {
    for (const auto& w : sample) {
      uint32_t server_index = hash_int(w.first) % num_servers;
      // uint32_t data_index = (w.first - server_index) / num_servers;
      uint32_t data_index = w.first;
      store_value<uint32_t>(msg_lst[server_index], data_index);
      num_weights_lst[server_index]++;
    }
  }

  for (int i = 0; i < num_servers; i++) {
    char* msg = msg_begin_lst[i];
    store_value(msg, num_weights_lst[i]);

    uint32_t operation = GET_LR_SPARSE_MODEL;
    if (send_all(psints[i]->sock, &operation, sizeof(uint32_t)) == -1) {
      throw std::runtime_error("Error getting sparse lr model");
    }

    // 2. Send msg size
    uint32_t msg_size =
        sizeof(uint32_t) + sizeof(uint32_t) * num_weights_lst[i];
    send_all(psints[i]->sock, &msg_size, sizeof(uint32_t));

    // 3. Send num_weights + weights
    if (send_all(psints[i]->sock, msg_begin_lst[i], msg_size) == -1) {
      throw std::runtime_error("Error getting sparse lr model");
    }
  }

  // we get the model subset with just the right amount of weights
  for (int i = 0; i < num_servers; i++) {
    psints[i]->get_lr_sparse_model_inplace_sharded(
        model, config, msg_begin_lst[i], num_weights_lst[i], i, num_servers);
    // delete[] msg_begin_lst[i];
  }
}

void MultiplePSSparseServerInterface::get_mf_sparse_model_inplace(
    const SparseDataset& ds,
    SparseMFModel& model,
    const Configuration& config,
    uint32_t user_base,
    uint32_t minibatch_size) {
  int num_servers = psints.size();

  std::unique_ptr<char*> msg_lst_ptr(new char*[num_servers]);
  std::unique_ptr<char*> msg_begin_lst_ptr(new char*[num_servers]);

  char** msg_lst = msg_lst_ptr.get();
  char** msg_begin_lst = msg_begin_lst_ptr.get();

  std::unique_ptr<uint32_t[]> item_ids_count_lst(new uint32_t[num_servers]);
  bool seen[config.get_items()] = {0};

  user_base /= num_servers;

  std::array<std::unique_ptr<char>, MAX_NUM_PS> msg_lst_i_ptr;
  for (int i = 0; i < num_servers; i++) {
    msg_lst_i_ptr[i] = std::unique_ptr<char>(new char[MAX_MSG_SIZE]);
    msg_lst[i] = msg_lst_i_ptr[i].get();
    msg_begin_lst[i] = msg_lst[i];
    item_ids_count_lst[i] = 0;
    store_value<uint32_t>(msg_lst[i], 0);  // We will write this value later
    store_value<uint32_t>(msg_lst[i], user_base);
    store_value<uint32_t>(msg_lst[i], minibatch_size / num_servers);
    store_value<uint32_t>(msg_lst[i], MAGIC_NUMBER);
  }

  std::vector<std::vector<uint32_t>> movie_memory(num_servers);

  for (const auto& sample : ds.data_) {
    for (const auto& w : sample) {
      uint32_t server_num = hash_int(w.first) % num_servers;
      uint32_t movieId = w.first;
      if (seen[movieId])
        continue;
      movie_memory[server_num].push_back(w.first);
      store_value<uint32_t>(msg_lst[server_num], movieId);
      seen[movieId] = true;
      item_ids_count_lst[server_num]++;
    }
  }

  // Send requests to all parameter servers
  for (int i = 0; i < num_servers; i++) {
    char* msg = msg_begin_lst[i];
    store_value<uint32_t>(msg, item_ids_count_lst[i]);

    uint32_t operation = GET_MF_SPARSE_MODEL;
    if (send_all(psints[i]->sock, &operation, sizeof(uint32_t)) == -1) {
      throw std::runtime_error("Error getting sparse mf model");
    }

    uint32_t msg_size =
        sizeof(uint32_t) * 4 + sizeof(uint32_t) * item_ids_count_lst[i];
    send_all(psints[i]->sock, &msg_size, sizeof(uint32_t));

    if (send_all(psints[i]->sock, msg_begin_lst[i], msg_size) == -1) {
      throw std::runtime_error("Error getting sparse mf model");
    }
  }

  // Receive responses from PS
  for (int i = 0; i < num_servers; i++) {
    psints[i]->get_mf_sparse_model_inplace_sharded(
        model, movie_memory[i], config, msg_begin_lst[i],
        minibatch_size / num_servers, item_ids_count_lst[i], i, num_servers);
  }
}

std::unique_ptr<CirrusModel> MultiplePSSparseServerInterface::get_full_model(
    const Configuration& config,
    bool use_col_filtering) {
  if (use_col_filtering) {
    std::unique_ptr<SparseMFModel> model = std::make_unique<SparseMFModel>(
        (uint64_t) 0, (uint64_t) 0, (uint64_t) NUM_FACTORS);
    for (int i = 0; i < psints.size(); i++) {
      psints[i]->get_full_model_inplace(model, config, i, psints.size());
    }
    return std::move(model);

  } else {
    std::unique_ptr<SparseLRModel> model = std::make_unique<SparseLRModel>(0);
    for (int i = 0; i < psints.size(); i++) {
      std::cout << "[ERROR_TASK] Calling inplace full model get" << std::endl;
      psints[i]->get_full_model_inplace(model, i, psints.size());
    }
    return std::move(model);
  }
}

}  // namespace cirrus
