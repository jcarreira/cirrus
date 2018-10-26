#include "MultiplePSSparseServerInterface.h"
#include <cassert>
#include "Constants.h"
#include "PSSparseServerInterface.h"

#undef DEBUG

#define MAX_MSG_SIZE (1024 * 1024)

namespace cirrus {

MultiplePSSparseServerInterface::MultiplePSSparseServerInterface(
    std::vector<std::string> param_ips,
    std::vector<uint64_t> ps_ports) {
  std::cout << "Starting Multiple PS " << param_ips.size() << std::endl;
  for (int i = 0; i < param_ips.size(); i++) {  // replace 2 with num_servers
    std::cout << "Attempting connection to " << param_ips[i] << ":"
              << ps_ports[i] << std::endl;
    auto ptr = new PSSparseServerInterface(param_ips[i], ps_ports[i]);
    while (true) {
      try {
        ptr->connect();
        break;
      } catch (std::exception& exc) {
        std::cout << exc.what();
      }
    }
    psints.push_back(ptr);
    std::cout << "Connected!!!" << std::endl;
  }
}

MultiplePSSparseServerInterface::MultiplePSSparseServerInterface(
    std::vector<std::string> param_ips,
    std::vector<uint64_t> ps_ports, uint32_t mb_size) {
  std::cout << "Starting Multiple PS " << param_ips.size() << std::endl;
  for (int i = 0; i < param_ips.size(); i++) {  // replace 2 with num_servers
    std::cout << "Attempting connection to " << param_ips[i] << ":"
              << ps_ports[i] << std::endl;
    auto ptr = new PSSparseServerInterface(param_ips[i], ps_ports[i]);
    while (true) {
      try {
        ptr->connect();
        break;
      } catch (std::exception& exc) {
        std::cout << exc.what();
      }
    }
    psints.push_back(ptr);
    std::cout << "Connected!!!" << std::endl;
  }
  minibatch_size = mb_size;
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
    ret = psint->send_wrapper(operation, sizeof(uint32_t));
    if (ret == -1)
      throw std::runtime_error("Error sending operation");
  }

  uint32_t size = gradient.getShardSerializedSize(num_ps);
  char data[size];
  auto starts_and_size = gradient.shard_serialize(data, num_ps);

  for (int i = 0; i < num_ps; i++) {
    auto psint = psints[i];
    auto sas = starts_and_size[i];

    ret = psint->send_wrapper(std::get<1>(sas), sizeof(uint32_t));
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
    ret = psint->send_wrapper(operation, sizeof(uint32_t));
    if (ret == -1)
      throw std::runtime_error("Error sending operation");
  }

  uint32_t size = gradient.getShardSerializedSize(num_ps);
  char data[size];
  auto starts_and_size = gradient.shard_serialize(data, num_ps, minibatch_size);

  for (int i = 0; i < num_ps; i++) {
    auto psint = psints[i];
    auto sas = starts_and_size[i];

    ret = psint->send_wrapper(std::get<1>(sas), sizeof(uint32_t));
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
  char** msg_lst = new char*[num_servers];
  char** msg_begin_lst = new char*[num_servers];
  uint32_t* num_weights_lst = new uint32_t[num_servers];
  for (int i = 0; i < num_servers; i++) {
    msg_lst[i] = new char[MAX_MSG_SIZE];
    msg_begin_lst[i] = msg_lst[i];
    num_weights_lst[i] = 0;
    store_value<uint32_t>(
        msg_lst[i],
        num_weights_lst[i]);  // just make space for the number of weights
  }

  // Split the dataset based on which server data belongs to.
  // XXX consider optimizing this

  for (const auto& sample : ds.data_) {
    for (const auto& w : sample) {
      uint32_t server_index = w.first % num_servers;
      uint32_t data_index = (w.first - server_index) / num_servers;
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
    delete[] msg_begin_lst[i];
  }

  delete[] msg_begin_lst;
  delete[] msg_lst;
  delete[] num_weights_lst;
}

SparseMFModel MultiplePSSparseServerInterface::get_mf_sparse_model(
    const SparseDataset& ds,
    const Configuration& config,
    uint32_t user_base,
    uint32_t minibatch_size) {
  SparseMFModel model((uint64_t) 0, (uint64_t) 0, (uint64_t) NUM_FACTORS);
  get_mf_sparse_model_inplace(ds, model, config, user_base, minibatch_size);
  return std::move(model);
}

void MultiplePSSparseServerInterface::get_mf_sparse_model_inplace(
    const SparseDataset& ds,
    SparseMFModel& model,
    const Configuration& config,
    uint32_t user_base,
    uint32_t minibatch_size) {
  int num_servers = psints.size();
  char** msg_lst = new char*[num_servers];
  char** msg_begin_lst = new char*[num_servers];
  uint32_t* item_ids_count_lst = new uint32_t[num_servers];
  bool** seen = new bool*[num_servers];

  user_base /= num_servers;

  for (int i = 0; i < num_servers; i++) {
    seen[i] = new bool[17770]();
    msg_lst[i] = new char[MAX_MSG_SIZE];
    msg_begin_lst[i] = msg_lst[i];
    item_ids_count_lst[i] = 0;
    store_value<uint32_t>(msg_lst[i], 0);  // We will right this value later
    store_value<uint32_t>(msg_lst[i], user_base);
    store_value<uint32_t>(msg_lst[i], minibatch_size / num_servers);
    store_value<uint32_t>(msg_lst[i], MAGIC_NUMBER);
    // user_base += minibatch_size / num_servers;
  }

  for (const auto& sample : ds.data_) {
    for (const auto& w : sample) {
      uint32_t server_num = w.first % num_servers;
      uint32_t movieId = w.first / num_servers;
      if (seen[server_num][movieId])
        continue;
      store_value<uint32_t>(msg_lst[server_num], movieId);
      seen[server_num][movieId] = true;
      item_ids_count_lst[server_num]++;
    }
  }

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

  for (int i = 0; i < num_servers; i++) {
    psints[i]->get_mf_sparse_model_inplace_sharded(
        model, config, msg_begin_lst[i], minibatch_size / num_servers,
        item_ids_count_lst[i], i, num_servers);
    delete[] msg_begin_lst[i];
  }
}

std::unique_ptr<CirrusModel> MultiplePSSparseServerInterface::get_full_model(
    bool use_col_filtering) {
  if (use_col_filtering) {
    std::unique_ptr<SparseMFModel> model = std::make_unique<SparseMFModel>(
        (uint64_t) 0, (uint64_t) 0, (uint64_t) NUM_FACTORS);
    for (int i = 0; i < psints.size(); i++) {
      psints[i]->get_full_model_inplace(model, i, psints.size());
    }
    return std::move(model);

  } else {
    std::unique_ptr<SparseLRModel> model = std::make_unique<SparseLRModel>(0);
    for (int i = 0; i < psints.size(); i++) {
      psints[i]->get_full_model_inplace(model, i, psints.size());
    }
    return std::move(model);
  }
}

}  // namespace cirrus
