#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "Constants.h"
#include "Checksum.h"
#include <signal.h>
#include "OptimizationMethod.h"
#include "AdaGrad.h"
#include "Momentum.h"
#include "SGD.h"
#include "Nesterov.h"
#include "S3.h"
#include "gamma.h"
#include <array>
#include <stdlib.h>

#undef DEBUG

#define MAX_CONNECTIONS (nworkers) // (2 x # workers + 1)
#define THREAD_MSG_BUFFER_SIZE 1000000

namespace cirrus {
  
PSSparseServerTask::PSSparseServerTask(uint64_t model_size,
                                       uint64_t batch_size,
                                       uint64_t samples_per_batch,
                                       uint64_t features_per_sample,
                                       uint64_t nworkers,
                                       uint64_t worker_id,
                                       const std::string& ps_ip,
                                       uint64_t ps_port)
    : MLTask(model_size,
             batch_size,
             samples_per_batch,
             features_per_sample,
             nworkers,
             worker_id,
             ps_ip,
             ps_port),
      kill_signal(false),
      main_thread(0),
      threads_barrier(new pthread_barrier_t, destroy_pthread_barrier) {
  std::cout << "PSSparseServerTask is built" << std::endl;

  std::atomic_init(&gradientUpdatesCount, 0UL);
  std::atomic_init(&thread_count, 0);

  operation_to_name[0] = "SEND_LR_GRADIENT";
  operation_to_name[1] = "SEND_MF_GRADIENT";
  operation_to_name[2] = "GET_LR_FULL_MODEL";
  operation_to_name[3] = "GET_MF_FULL_MODEL";
  operation_to_name[4] = "GET_LR_SPARSE_MODEL";
  operation_to_name[5] = "GET_MF_SPARSE_MODEL";
  operation_to_name[6] = "SET_TASK_STATUS";
  operation_to_name[7] = "GET_TASK_STATUS";
  operation_to_name[8] = "REGISTER_TASK";
  operation_to_name[9] = "GET_NUM_CONNS";
  operation_to_name[13] = "SEND_LDA_UPDATE";
  operation_to_name[14] = "GET_LDA_MODEL";
  operation_to_name[15] = "GET_LDA_SLICES_IDX";
  operation_to_name[16] = "SEND_LL_NDT";

  for (int i = 0; i < NUM_PS_WORK_THREADS; i++) {
    thread_msg_buffer[i].reset(new char[THREAD_MSG_BUFFER_SIZE]);
  }
}

std::shared_ptr<char> PSSparseServerTask::serialize_lr_model(
    const SparseLRModel& lr_model, uint64_t* model_size) const {
  *model_size = lr_model.getSerializedSize();
  auto d = std::shared_ptr<char>(
      new char[*model_size], std::default_delete<char[]>());
  lr_model.serializeTo(d.get());
  return d;
}

bool PSSparseServerTask::testRemove(struct pollfd x, int poll_id) {
  // If this pollfd will be removed, the index of the next location to insert
  // should be reduced by one correspondingly.
  if (x.fd == -1) {
    curr_indexes[poll_id] -= 1;
  }
  return x.fd == -1;
}

bool PSSparseServerTask::process_send_mf_gradient(
    const Request& req,
    std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
#ifdef DEBUG
  std::cout << "APPLY_GRADIENT_REQ incoming size: " << incoming_size
            << std::endl;
#endif
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }
  if (read_all(req.sock, thread_buffer.data(), incoming_size) == 0) {
    return false;
  }

  MFSparseGradient gradient;
  gradient.loadSerialized(thread_buffer.data());

  model_lock.lock();
#ifdef DEBUG
  std::cout << "Doing sgd update" << std::endl;
#endif
  mf_model->sgd_update(
      task_config.get_learning_rate(), &gradient);
#ifdef DEBUG
  std::cout
    << "sgd update done"
    << " checksum: " << mf_model->checksum()
    << std::endl;
#endif
  model_lock.unlock();
  gradientUpdatesCount++;
  return true;
}

bool PSSparseServerTask::process_send_lr_gradient(
    const Request& req,
    std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
#ifdef DEBUG
  std::cout << "APPLY_GRADIENT_REQ incoming size: " << incoming_size
            << std::endl;
#endif
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }
  //buffer.resize(incoming_size);
  try {
    if (read_all(req.sock, thread_buffer.data(), incoming_size) == 0) {
      return false;
    }
  } catch (...) {
    throw std::runtime_error("Uhandled error");
  }

  LRSparseGradient gradient(0);
  gradient.loadSerialized(thread_buffer.data());

  model_lock.lock();
  opt_method->sgd_update(
      lr_model, &gradient);
  model_lock.unlock();
  gradientUpdatesCount++;
  return true;
}

bool PSSparseServerTask::process_send_lda_update(
    const Request& req,
    std::vector<char>& thread_buffer) {

  uint32_t incoming_size = req.incoming_size;
#ifdef DEBUG
  std::cout << "APPLY_GRADIENT_REQ incoming size: " << incoming_size
            << std::endl;
#endif
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }

  int tokens;
  read_all(req.sock, &tokens, sizeof(int));
  tokens_sampled += tokens;

  try {
    if (read_all(req.sock, thread_buffer.data(), incoming_size - sizeof(int)) == 0) {
      return false;
    }
  } catch (...) {
    throw std::runtime_error("Uhandled error");
  }

  receive_size += ((double) incoming_size) / 1000000.;

  const char* data = thread_buffer.data();

  int update_bucket = lda_global_vars->update(data);
  gradientUpdatesCount++;
  return true;
}

// XXX we have to refactor this ASAP
// move this to SparseMFModel

bool PSSparseServerTask::process_get_mf_sparse_model(
    const Request& req, std::vector<char>& thread_buffer, int thread_number) {
  uint32_t k_items = 0;
  uint32_t base_user_id = 0;
  uint32_t minibatch_size = 0;
  uint32_t magic_value = 0;

  read_all(req.sock, &k_items, sizeof(uint32_t));
  read_all(req.sock, &base_user_id, sizeof(uint32_t));
  read_all(req.sock, &minibatch_size, sizeof(uint32_t));
  read_all(req.sock, &magic_value, sizeof(uint32_t));

  assert(k_items > 0);
  assert(minibatch_size > 0);
  if (magic_value != MAGIC_NUMBER) {
    throw std::runtime_error("Wrong message");
  }
  read_all(req.sock, thread_buffer.data(), k_items * sizeof(uint32_t));
  uint32_t to_send_size =
      minibatch_size *
          (sizeof(uint32_t) + (NUM_FACTORS + 1) * sizeof(FEATURE_TYPE)) +
      k_items * (sizeof(uint32_t) + (NUM_FACTORS + 1) * sizeof(FEATURE_TYPE));
#ifdef DEBUG
  std::cout << "k_items: " << k_items << std::endl;
  std::cout << "base_user_id: " << base_user_id << std::endl;
  std::cout << "minibatch_size: " << minibatch_size << std::endl;
#endif

  SparseMFModel sparse_mf_model((uint64_t) 0, 0, 0);
  sparse_mf_model.serializeFromDense(*mf_model, base_user_id, minibatch_size,
                                     k_items, thread_buffer.data(),
                                     thread_msg_buffer[thread_number].get());
  //uint32_t to_send_size = data_to_send.size();
  if (send_all(req.sock, &to_send_size, sizeof(uint32_t)) == -1) {
    return false;
  }
  if (send_all(req.sock, thread_msg_buffer[thread_number].get(),
               to_send_size) == -1) {
    return false;
  }
  return true;
}

bool PSSparseServerTask::process_get_lr_sparse_model(
    const Request& req, std::vector<char>& thread_buffer) {
  // need to parse the buffer to get the indices of the model we want
  // to send back to the client
  uint32_t incoming_size = req.incoming_size;
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }
#ifdef DEBUG
  std::cout << "GET_MODEL_REQ incoming size: " << incoming_size << std::endl;
#endif
  try {
    if (read_all(req.sock, thread_buffer.data(), incoming_size) == 0) {
      return false;
    }
  } catch (...) {
    throw std::runtime_error("Uhandled error");
  }

  const char* data = thread_buffer.data();
  uint64_t num_entries = load_value<uint32_t>(data);

  uint32_t to_send_size = num_entries * sizeof(FEATURE_TYPE);
  assert(to_send_size < 1024 * 1024);
  char data_to_send[1024 * 1024]; // 1MB
  char* data_to_send_ptr = data_to_send;

#ifdef DEBUG
  std::cout << "Sending back: " << num_entries
    << " weights from model. Size: " << to_send_size
    << std::endl;
#endif
  for (uint32_t i = 0; i < num_entries; ++i) {
    uint32_t entry_index = load_value<uint32_t>(data);
    double weight = lr_model->get_nth_weight(entry_index);
    opt_method->edit_weight(weight);
    store_value<FEATURE_TYPE>(
        data_to_send_ptr,
        weight);
  }
  if (send_all(req.sock, data_to_send, to_send_size) == -1) {
    return false;
  }
  return true;
}

bool PSSparseServerTask::process_get_lda_model(
    const Request& req,
    std::vector<char>& thread_buffer) {

  auto start_time_benchmark = get_time_ms();
  auto start_time_temp = get_time_ms();

  int previous_slice_id, local_model_id;
  read_all(req.sock, &previous_slice_id, sizeof(int));
  read_all(req.sock, &local_model_id, sizeof(int));

  task_id_lookup[req.poll_fd.fd] = local_model_id - task_config.get_train_range().first;

  slice_lock.lock();

  start_time_temp = get_time_ms();

  int rand_max = unused_slice_id.size();

  // determine the next word slice to send
  int id_to_send, slice_id_to_send;
  if (unused_slice_id.size() == 0) {
    slice_id_to_send = previous_slice_id;
  } else {
    id_to_send = std::rand() % (rand_max);
    slice_id_to_send = unused_slice_id[id_to_send];
    unused_slice_id.erase(unused_slice_id.begin() + id_to_send);
    if (previous_slice_id != -1) {
      unused_slice_id.push_back(previous_slice_id);
    }
  }
  sock_lookup[req.poll_fd.fd] = slice_id_to_send;
  slice_lock.unlock();

  // serialize the word slice
  uint32_t to_send_size, uncompressed_size;
  start_time_temp = get_time_ms();
  auto pure_partial_benchmark = get_time_ms();
  char* data_to_send;
  data_to_send = lda_global_vars->get_partial_model(slice_id_to_send, to_send_size, uncompressed_size, local_model_id);

  time_find_partial += (get_time_ms() - start_time_temp) / 1000.0;
  num_to_find_partial += 1.;
  start_time_temp = get_time_ms();

  // send the size of compressed partial model
  if (send_all(req.sock, &to_send_size, sizeof(uint32_t)) == -1) {
    return false;
  }

  // send the size of original partial model
  if (send_all(req.sock, &uncompressed_size, sizeof(uint32_t)) == -1) {
    return false;
  }

  // send the slice id
  if (send_all(req.sock, &slice_id_to_send, sizeof(uint32_t)) == -1) {
    return false;
  }

  time_send_sizes += (get_time_ms() - start_time_temp) / 1000.0;

  start_time_temp = get_time_ms();
  // send the partial model
  if (send_all(req.sock, data_to_send, to_send_size) == -1) {
    return false;
  }

  // time_process_get += (get_time_ms() - start_time_benchmark) / 1000.0;
  time_send_partial += (get_time_ms() - start_time_temp) / 1000.0;

  time_whole += (get_time_ms() - start_time_benchmark) / 1000.0;

  send_size += ((double) to_send_size) / 1000000.;

  delete data_to_send;
  return true;
}

bool PSSparseServerTask::process_get_slices_indices(
    const Request& req,
    std::vector<char>& thread_buffer) {

  int local_model_id;
  read_all(req.sock, &local_model_id, sizeof(int));

  uint32_t to_send_size;
  auto train_range = task_config.get_train_range();
  char* data_to_send = lda_global_vars->get_slices_indices(local_model_id - train_range.first, to_send_size);

  if (send_all(req.sock, &to_send_size, sizeof(uint32_t)) == -1) {
    return false;
  }

  if (send_all(req.sock, data_to_send, to_send_size) == -1) {
    return false;
  }

  delete data_to_send;
  return true;
}

bool PSSparseServerTask::process_send_ll_update(
    const Request& req,
    std::vector<char>& thread_buffer) {

  int bucket_id;
  if (read_all(req.sock, &bucket_id, sizeof(int)) == -1) {
    return false;
  }

  double ll;
  if (read_all(req.sock, &ll, sizeof(double)) == -1) {
    return false;
  }

  auto train_range = task_config.get_train_range();
  ll_lock.lock();
  ll_ndt[bucket_id - train_range.first] = ll;
  ll_lock.unlock();

  return true;
}


bool PSSparseServerTask::process_get_mf_full_model(
    const Request& req, std::vector<char>& thread_buffer) {
  model_lock.lock();
  auto mf_model_copy = *mf_model;
  model_lock.unlock();
  uint32_t model_size = mf_model_copy.getSerializedSize();

  if (thread_buffer.size() < model_size) {
    std::cout << "thread_buffer.size(): " << thread_buffer.size()
      << " model_size: " << model_size << std::endl;
    throw std::runtime_error("Thread buffer too small");
  }

  mf_model_copy.serializeTo(thread_buffer.data());
  std::cout
    << "Serializing mf model"
    << " mode checksum: " << mf_model_copy.checksum()
    << " buffer checksum: " << crc32(thread_buffer.data(), model_size)
    << std::endl;
  if (send_all(req.sock, &model_size, sizeof(uint32_t)) == -1) {
    return false;
  }
  if (send_all(req.sock, thread_buffer.data(), model_size) == -1) {
    return false;
  }
  return true;
}

bool PSSparseServerTask::process_get_lr_full_model(
    const Request& req, std::vector<char>& thread_buffer) {
  model_lock.lock();
  auto lr_model_copy = *lr_model;
  model_lock.unlock();

  // TODO: This should be largest non-zero weight in model. That way
  // we can reduce the model size, espeically for a large model split across
  // multiple PS
  uint32_t model_size = lr_model_copy.getSerializedSize();

  if (thread_buffer.size() < model_size) {
    std::string error_str = "buffer with size " +
                            std::to_string(thread_buffer.size()) +
                            "too small: " + std::to_string(model_size);
    throw std::runtime_error(error_str);
  }

  lr_model_copy.serializeTo(thread_buffer.data());
  if (send_all(req.sock, thread_buffer.data(), model_size) == -1)
    return false;
  return true;
}

void PSSparseServerTask::handle_failed_read(struct pollfd* pfd) {
  if (close(pfd->fd) != 0) {
    std::cout << "Error closing socket. errno: " << errno << std::endl;
  }
  num_connections--;
  if (sock_lookup[pfd->fd] != -1) {
    unused_slice_id.push_back(sock_lookup[pfd->fd]);
    sock_lookup[pfd->fd] = -1;
  }
  std::cout << "PS closing connection after process(): " << num_connections
            << std::endl;
  std::cout << "Task id: " << task_id_lookup[pfd->fd] + 5 << std::endl;
  pfd->fd = -1;
  pfd->revents = 0;
}

void PSSparseServerTask::gradient_f() {
  std::vector<char> thread_buffer;
  thread_buffer.resize(120 * 1024 * 1024); // 120 MB
  struct timespec ts;
  int thread_number = thread_count++;
  while (!kill_signal) {
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
      throw std::runtime_error("Error in clock_gettime");
    }
    ts.tv_sec += 3;
    int s = sem_timedwait(&sem_new_req, &ts);

    if (s == -1) {  // if sem_wait timed out and kill signal is not set, restart
                    // the while loop
      if (!kill_signal)
        continue;
      else
        break;
    }

    to_process_lock.lock();
    Request req = std::move(to_process.front());

    to_process.pop();
    to_process_lock.unlock();
    int sock = req.poll_fd.fd;

    // first read 4 bytes for operation ID
    uint32_t operation = 0;
    if (read_all(sock, &operation, sizeof(uint32_t)) == 0) {
      handle_failed_read(&req.poll_fd);
      continue;
    }

#ifdef DEBUG
    std::cout << "Operation: " << operation << " - "
              << operation_to_name[operation] << std::endl;
#endif

    if (operation == REGISTER_TASK) {
      // read the task id
      uint32_t task_id = 0;
      if (read_all(sock, &task_id, sizeof(uint32_t)) == 0) {
        handle_failed_read(&req.poll_fd);
        continue;
      }
      // check if this task has already been registered
      uint32_t task_reg =
          (registered_tasks.find(task_id) != registered_tasks.end());

      if (task_reg == 0) {
        registered_tasks.insert(task_id);
        num_connections -= 1;
      }
      send_all(sock, &task_reg, sizeof(uint32_t));

      // if (task_reg == 1){
      //   num_connections -= 1;
      // }
      continue;
    } else if (operation == SEND_LR_GRADIENT || operation == SEND_MF_GRADIENT ||
               operation == GET_LR_SPARSE_MODEL ||
               operation == GET_MF_SPARSE_MODEL ||
               operation == SEND_LDA_UPDATE || operation == GET_LDA_MODEL ||
               operation == GET_LDA_SLICES_IDX || operation == SEND_LL_NDT) {
      // read 4 bytes of the size of the remaining message
      uint32_t incoming_size = 0;
      if (read_all(sock, &incoming_size, sizeof(uint32_t)) == 0) {
        handle_failed_read(&req.poll_fd);
        continue;
      }
      req.incoming_size = incoming_size;
    }

    if (operation == SEND_LR_GRADIENT) {
      if (!process_send_lr_gradient(req, thread_buffer)) {
        break;
      }
    } else if (operation == SEND_MF_GRADIENT) {
      if (!process_send_mf_gradient(req, thread_buffer)) {
        break;
      }
    } else if (req.req_id == SEND_LDA_UPDATE) {
      if (!process_send_lda_update(req, thread_buffer)) {
        break;
      }
    } else if (req.req_id == GET_LR_SPARSE_MODEL) {
#ifdef DEBUG
      std::cout << "process_get_lr_sparse_model" << std::endl;
      auto before = get_time_us();
#endif
      if (!process_get_lr_sparse_model(req, thread_buffer)) {
        break;
      }
#ifdef DEBUG
      auto elapsed = get_time_us() - before;
      std::cout << "GET_LR_SPARSE_MODEL Elapsed(us): " << elapsed << std::endl;
#endif
    } else if (operation == GET_MF_SPARSE_MODEL) {
      if (!process_get_mf_sparse_model(req, thread_buffer, thread_number)) {
        break;
      }
    } else if (operation == GET_LR_FULL_MODEL) {
      if (!process_get_lr_full_model(req, thread_buffer))
        break;
    } else if (operation == GET_MF_FULL_MODEL) {
      if (!process_get_mf_full_model(req, thread_buffer))
        break;
    } else if (req.req_id == GET_LDA_MODEL) {
      if (!process_get_lda_model(req, thread_buffer))
        break;
    } else if (req.req_id == GET_LDA_SLICES_IDX) {
      if (!process_get_slices_indices(req, thread_buffer))
        break;
    } else if (req.req_id == SEND_LL_NDT) {
      if (!process_send_ll_update(req, thread_buffer))
        break;
    } else if (req.req_id == GET_TASK_STATUS) {
      uint32_t task_id;
      if (read_all(sock, &task_id, sizeof (uint32_t)) == 0) {
        break;
      }
#ifdef DEBUG
      std::cout << "Get status task id: " << task_id << std::endl;
#endif
      assert(task_id < 10000);
      if (task_to_status.find(task_id) == task_to_status.end() ||
          task_to_status[task_id] == false) {
        uint32_t status = 0;
        send_all(sock, &status, sizeof (uint32_t));
      } else {
        uint32_t status = 1;
        send_all(sock, &status, sizeof (uint32_t));
      }

    } else if (operation == SET_TASK_STATUS) {
      uint32_t data[2] = {0}; // id + status
      if (read_all(sock, data, sizeof (uint32_t) * 2) == 0) {
        handle_failed_read(&req.poll_fd);
        continue;
      }
#ifdef DEBUG
      std::cout << "Set status task id: " << data[0] << " status: " << data[1]
                << std::endl;
#endif
      task_to_status[data[0]] = data[1];

    } else if (operation == GET_NUM_CONNS) {
      // NOTE: Consider changing this to flatbuffer serialization?
      std::cout << "Retrieve info: " << num_connections << std::endl;
      if (send(sock, &num_connections, sizeof(uint32_t), 0) < 0) {
        throw std::runtime_error("Error sending number of connections");
      }
    } else if (operation == GET_NUM_UPDATES) {
      std::cout << "Retrieve info: " << num_updates << std::endl;
      if (send(sock, &num_updates, sizeof(uint32_t), 0) < 0) {
        throw std::runtime_error("Error sending number of connections");
      }
    } else if (operation == KILL_SIGNAL) {
      std::cout << "Received kill signal!" << std::endl;
      kill_server();
      break;
    } else {
      throw std::runtime_error("gradient_f: Unknown operation");
    }

    // We reactivate events from the client socket here
    req.poll_fd.events = POLLIN;
    //pthread_kill(main_thread, SIGUSR1);

    assert(write(pipefds[req.id][1], "a", 1) == 1); // wake up poll()

#ifdef DEBUG
    std::cout << "gradient_f done" << std::endl;
#endif
  }

  std::cout << "Gradient F is ending" << std::endl;
}

/**
 * FORMAT
 * incoming size (uint32_t)
 * buffer with previous size
 */
bool PSSparseServerTask::process(struct pollfd& poll_fd, int thread_id) {
  int sock = poll_fd.fd;
#ifdef DEBUG
  std::cout << "Processing socket: " << sock << std::endl;
#endif

  uint32_t incoming_size = 0;
#ifdef DEBUG
  std::cout << "incoming size: " << incoming_size << std::endl;
#endif
  to_process_lock.lock();
  poll_fd.events = 0; // explain this
  to_process.push(Request(sock, thread_id, incoming_size, poll_fd));
  to_process_lock.unlock();
  sem_post(&sem_new_req);
  return true;
}

void PSSparseServerTask::start_server() {
  lr_model.reset(new SparseLRModel(model_size));
  lr_model->randomize();
  mf_model.reset(new MFModel(task_config.get_users(), task_config.get_items(),
                             NUM_FACTORS));
  mf_model->randomize();
  if (task_config.get_model_type() == cirrus::Configuration::LDA) {
    std::cout << "Getting initial LDAUpdate from S3\n";
    // Get the global stats from S3
    s3_initialize_aws();
    std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();
    std::string obj_id_str =
        std::to_string(hash_f(std::to_string(0).c_str())) + "-LDA";
    std::ostringstream* s3_obj =
        s3_client->s3_get_object_ptr(obj_id_str, task_config.get_s3_bucket());
    const std::string tmp = s3_obj->str();
    const char* s3_data = tmp.c_str();

    s3_shutdown_aws();

    lda_global_vars.reset(new LDAUpdates());
    lda_global_vars->loadSerialized(s3_data);
    init_loglikelihood();
    std::cout << "Finished getting initial statistics.\n";


    int slice_size = task_config.get_slice_size();

    pre_assign_slices(slice_size);

    std::srand(std::time(nullptr));
    sock_lookup.fill(-1);
    bucket_in_update.fill(-1);
    task_id_lookup.fill(-1);
  }

  mf_model.reset(new MFModel(task_config.get_users(), task_config.get_items(),
                             NUM_FACTORS));
  mf_model->randomize();

  sem_init(&sem_new_req, 0, 0);

  for (uint32_t i = 0; i < NUM_PS_WORK_THREADS; ++i) {
    gradient_thread.push_back(std::make_unique<std::thread>(
          std::bind(&PSSparseServerTask::gradient_f, this)));
  }

  // create barrier for all poll threads
  if (pthread_barrier_init(threads_barrier.get(), nullptr, NUM_POLL_THREADS) !=
      0) {
    throw std::runtime_error("Error in threads barrier");
  }

  for (int i = 0; i < NUM_POLL_THREADS; i++) {
    server_threads.push_back(std::make_unique<std::thread>(
        std::bind(&PSSparseServerTask::main_poll_thread_fn, this, i)));
  }

  // start checkpoing thread
  if (task_config.get_checkpoint_frequency() > 0) {
      checkpoint_thread.push_back(std::make_unique<std::thread>(
                  std::bind(&PSSparseServerTask::checkpoint_model_loop, this)));
  }
}

void PSSparseServerTask::kill_server() {
  kill_signal = 1;
}

void PSSparseServerTask::main_poll_thread_fn(int poll_id) {
  // id=0 -> poll thread responsible for handling new connections
  if (poll_id == 0) {
    std::cout << "Starting server, poll id " << poll_id << std::endl;

    server_sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_ < 0) {
      throw std::string("Server error creating socket");
    }

    int opt = 1;
    if (setsockopt(server_sock_, IPPROTO_TCP,
                TCP_NODELAY, &opt, sizeof(opt))) {
      throw std::runtime_error("Error setting socket options.");
    }
    if (setsockopt(server_sock_, SOL_SOCKET,
                SO_REUSEADDR, &opt, sizeof(opt))) {
      throw std::runtime_error("Error forcing port binding");
    }

    if (setsockopt(server_sock_, SOL_SOCKET,
                SO_REUSEPORT, &opt, sizeof(opt))) {
      throw std::runtime_error("Error forcing port binding");
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(ps_port);
    std::memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

    int ret = bind(server_sock_,
            reinterpret_cast<sockaddr*> (&serv_addr), sizeof(serv_addr));
    if (ret < 0) {
      throw std::runtime_error("Error binding in port " + to_string(ps_port));
    }

    if (listen(server_sock_, SOMAXCONN) == -1) {
      throw std::runtime_error("Error listening on port " + to_string(ps_port));
    }
    fdses[0].at(0).fd = server_sock_;
    fdses[0].at(0).events = POLLIN;
    fdses[0].at(1).fd = pipefds[poll_id][0];
    fdses[0].at(1).events = POLLIN;
    curr_indexes[poll_id] = 2;
  } else {
    std::cout << "Starting secondary poll thread: " << poll_id << std::endl;
    fdses[poll_id].at(0).fd = pipefds[poll_id][0];
    fdses[poll_id].at(0).events = POLLIN;
    curr_indexes[poll_id] = 1;

  }

  pthread_barrier_wait(threads_barrier.get());

  loop(poll_id);
}

void PSSparseServerTask::loop(int poll_id) {

  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);

  // buffer.resize(10 * 1024 * 1024); // reserve 10MB upfront

  std::cout << "Starting loop for id: " << poll_id << std::endl;
  while (!kill_signal) {
    int poll_status =
        poll(fdses[poll_id].data(), curr_indexes[poll_id], timeout);
    if (poll_status == -1) {
      if (errno != EINTR) {
        throw std::runtime_error("Server error calling poll.");
      } else {
        std::cout << "EINTR" << std::endl;
      }
    } else if (
            (poll_id == 0 && fdses[poll_id][1].revents == POLLIN)
         || (poll_id != 0 && fdses[poll_id][0].revents == POLLIN)) {
      int posit = (poll_id == 0);        // =1 if main poll thread, 0 otherwise
      fdses[poll_id][posit].revents = 0; // Reset the event flags
      char a[1];
      assert(read(pipefds[poll_id][0], a, 1) >= 0);
      // ignore
    } else if (poll_status == 0) {
    } else {
      // there is at least one pending event, find it.
      for (uint64_t i = 0; i < curr_indexes[poll_id]; i++) {
        struct pollfd& curr_fd = fdses[poll_id][i];
        // Ignore the fd if we've said we don't care about it
        if (curr_fd.fd == -1) {
          continue;
        }
        if (curr_fd.revents != POLLIN) {
          //LOG<ERROR>("Non read event on socket: ", curr_fd.fd);
          if (curr_fd.revents & POLLHUP) {
            std::cout << "PS closing connection " << num_connections
                      << std::endl;
            close(curr_fd.fd);
            curr_fd.fd = -1;
          }
        } else if (poll_id == 0 && curr_fd.fd == server_sock_) {
          std::cout << poll_id << std::endl;
          std::cout << "PS new connection!" << std::endl;
          int newsock = accept(server_sock_,
              reinterpret_cast<struct sockaddr*> (&cli_addr),
              &clilen);
          if (poll_id == 0 && newsock < 0) {
            throw std::runtime_error("Error accepting socket");
          }
          // If at capacity, reject connection
          if (poll_id == 0 && num_connections > (MAX_CONNECTIONS - 1)) {
            std::cout << "Rejecting connection "
              << num_connections
              << std::endl;
            close(newsock);
          } else if (poll_id == 0 && curr_indexes[poll_id] == max_fds) {
            throw std::runtime_error("We reached capacity");
            close(newsock);
          } else if (poll_id == 0) {

            int r = rand() % NUM_POLL_THREADS;
            std::cout << "Random: " << r << std::endl;
            fdses[r][curr_indexes[r]].fd = newsock;
            fdses[r][curr_indexes[r]].events = POLLIN;
            curr_indexes[r]++;
            num_connections++;

          }
        } else {
#ifdef DEBUG
          std::cout << "Calling process" << std::endl;
#endif
          if (!process(curr_fd, poll_id)) {
            if (close(curr_fd.fd) != 0) {
              std::cout << "Error closing socket. errno: " << errno
                        << std::endl;
            }
            num_connections--;
            std::cout << "PS closing connection after process(): "
                      << num_connections << std::endl;
            curr_fd.fd = -1;
          }
        }
        curr_fd.revents = 0; // Reset the event flags
      }
    }
    // If at max capacity, try to make room
    if (curr_indexes[poll_id] == max_fds) {
      // Try to purge unused fds, those with fd == -1
      std::cout << "Purging" << std::endl;
      std::remove_if(fdses[poll_id].begin(), fdses[poll_id].end(),
          std::bind(&PSSparseServerTask::testRemove,
              this, std::placeholders::_1, poll_id));
    }
  }
}

void sig_handler(int) {
  //std::cout << "Sig handler" << std::endl;
}

/**
 * This is the task that runs the parameter server
 * This task is responsible for
 * 1) sending the model to the workers
 * 2) receiving the gradient updates from the workers
 *
 */
void PSSparseServerTask::run(const Configuration& config) {
  std::cout
    << "PS task initializing model"
    << std::endl;

  for (int i = 0; i < NUM_POLL_THREADS; i++) {
    assert(pipe(pipefds[i]) != -1);
    curr_indexes[i] = 0;
    fdses[i].resize(max_fds);
  }

  main_thread = pthread_self();
  if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
    throw std::runtime_error("Unable to set signal handler");
  }

  task_config = config;

  if (task_config.get_model_type() != cirrus::Configuration::LDA) {
    auto learning_rate = config.get_learning_rate();
    auto epsilon = config.get_epsilon();
    auto momentum_beta = config.get_momentum_beta();
    if (config.get_opt_method() == "sgd") {
      opt_method.reset(new SGD(learning_rate));
    } else if (config.get_opt_method() == "nesterov") {
      opt_method.reset(new Nesterov(learning_rate, momentum_beta));
    } else if (config.get_opt_method() == "momentum") {
      opt_method.reset(new Momentum(learning_rate, momentum_beta));
    } else if (config.get_opt_method() == "adagrad") {
      opt_method.reset(new AdaGrad(learning_rate, epsilon));
    } else {
      throw std::runtime_error("Unknown opt method");
    }
  }
  start_server();

  //wait_for_start(PS_SPARSE_SERVER_TASK_RANK, redis_con, nworkers);

  uint64_t start = get_time_us();
  uint64_t last_tick = get_time_us();

  while (!kill_signal) {
    auto now = get_time_us();
    auto elapsed_us = now - last_tick;
    auto since_start_sec = 1.0 * (now - start) / 1000000;

    num_updates = static_cast<uint32_t>(1.0 * gradientUpdatesCount /
                                        elapsed_us * 1000 * 1000);

    if (elapsed_us > 1000000) {
      last_tick = now;
      std::cout << "Events in the last sec: "
        << 1.0 * gradientUpdatesCount / elapsed_us * 1000 * 1000
        << " since (sec): " << since_start_sec
        << " #conns: " << num_connections
        << std::endl;
      gradientUpdatesCount = 0;

      if ((int) since_start_sec % 10 == 0) {
        compute_loglikelihood();
      }
    }
    sleep(1);
  }

  for (auto& thread : server_threads) {
    std::cout << "Joining poll thread" << std::endl;
    thread.get()->join();
  }

  for (auto& thread : gradient_thread) {
    std::cout << "Joining gradient thread" << std::endl;
    thread.get()->join();
  }
  for (auto& thread : checkpoint_thread) {
    std::cout << "Joining check thread" << std::endl;
    thread.get()->join();
  }
}

void PSSparseServerTask::checkpoint_model_loop() {
    if (task_config.get_checkpoint_frequency() == 0) {
        // checkpoint disabled
        return;
    }

    while (!kill_signal) {
      sleep(task_config.get_checkpoint_frequency());
      // checkpoint to s3
    }
}

void PSSparseServerTask::checkpoint_model_file(
    const std::string& filename) const {
  uint64_t model_size;
  std::shared_ptr<char> data = serialize_lr_model(*lr_model, &model_size);

  std::ofstream fout(filename.c_str(), std::ofstream::binary);
  fout.write(data.get(), model_size);
  fout.close();
}

double PSSparseServerTask::compute_loglikelihood() {

  double ll = ll_base;
  ll_lock.lock();
  for(int i=0; i<ll_ndt.size(); ++i){
    ll += ll_ndt[i];
  }
  ll_lock.unlock();
  compute_ll_threads.push_back(std::make_unique<std::thread>(
        std::bind(&PSSparseServerTask::update_ll_word_thread, this,
                  std::placeholders::_1), ll)
  );
  return ll;
}

void PSSparseServerTask::init_loglikelihood(){

  std::shared_ptr<std::vector<int>> nvt_ptr, nt_ptr;
  lda_global_vars->get_nvt_pointer(nvt_ptr);
  lda_global_vars->get_nt_pointer(nt_ptr);

  double alpha = 0.1, eta = .01;

  K = nt_ptr->size();
  V = nvt_ptr->size() / K;
  lgamma_eta = lda_lgamma(eta);
  lgamma_alpha = lda_lgamma(alpha);

  ll_base += K * lda_lgamma(eta * V);


  s3_initialize_aws();
  std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();

  auto train_range = task_config.get_train_range();
  ll_ndt.clear();
  ll_ndt.resize(train_range.second - train_range.first);

  int i = train_range.first;
  double ll_temp = 0.0;
  std::string obj_id_str =
      std::to_string(hash_f(std::to_string(i).c_str())) + "-LDA";
  std::ostringstream* s3_obj =
      s3_client->s3_get_object_ptr(obj_id_str, task_config.get_s3_bucket());

  const std::string tmp = s3_obj->str();
  const char* s3_data = tmp.c_str();

  LDAStatistics ndt_partial(s3_data);
  std::vector<std::vector<int>> ndt;
  ndt_partial.get_ndt(ndt);

  std::cout << "ndt size: " << ndt.size() << std::endl;

  for (int j = 0; j < ndt.size(); ++j) {
    int ndj = 0;
    for (int k = 0; k < K; ++k) {
      ndj += ndt[j][k];
      if (ndt[j][k] > 0) {
        ll_temp += lda_lgamma(alpha + ndt[j][k]) - lgamma_alpha;
      }
    }
    ll_temp += lda_lgamma(alpha * K) - lda_lgamma(alpha * K + ndj);
  }
  ll_ndt.clear();
  ll_ndt = std::vector<double>(train_range.second - train_range.first, ll_temp);

  start_time = get_time_ms();
  start_time_tokens = get_time_ms();
  compute_loglikelihood();

}

void PSSparseServerTask::update_ll_word_thread(double ll) {

  std::shared_ptr<std::vector<int>> nvt_ptr, nt_ptr;

  lda_global_vars->get_nvt_pointer(nvt_ptr);
  lda_global_vars->get_nt_pointer(nt_ptr);

  double current_time = (get_time_ms() - start_time) / 1000.0;
  double alpha = 0.1, eta = .01;

  K = nt_ptr->size();
  V = nvt_ptr->size() / K;
  lgamma_eta = lda_lgamma(eta);
  lgamma_alpha = lda_lgamma(alpha);

  double ll_word = 0.0;

  for (int i = 0; i < K; ++i) {
    ll_word -= lda_lgamma(eta * V + nt_ptr->operator[](i));
    for (int v = 0; v < V; ++v) {
      if (nvt_ptr->operator[](v * K + i) != 0) {
        ll_word += lda_lgamma(eta + nvt_ptr->operator[](v * K + i)) - lgamma_eta;
      }
    }
  }

  std::cout << "----------------------------------------------------------\n";
  std::cout << "**log-likelihood: " << ll + ll_word << " " <<  (get_time_ms() - start_time) / 1000.0 << std::endl;
  std::cout << "word ll: " << ll_word << std::endl;
  std::cout << "doc ll" << ll << std::endl;
  std::cout << "**tokens/sec: " << (double) tokens_sampled / ((get_time_ms() - start_time_tokens) / 1000.0) << std::endl;
  std::cout << "**send(mbs)/sec: " << (double) send_size / ((get_time_ms() - start_time_tokens) / 1000.0) << std::endl;
  std::cout << "**receive(mbs)/sec: " << (double) receive_size / ((get_time_ms() - start_time_tokens) / 1000.0) << std::endl;
  std::cout << "time: " << current_time << std::endl;
  std::cout << "----------------------------------------------------------\n";
  std::cout << "Avg Time (get_lda_model) function: " << time_whole / num_to_find_partial << std::endl;
  std::cout << "Avg Time to find partial model: " << time_find_partial / num_to_find_partial << std::endl;
  std::cout << "Avg Time to find partial model (excluding waiting): " << time_pure_find_partial / num_to_find_partial << std::endl;
  std::cout << "Avg Time to send the sizes: " << time_send_sizes / num_to_find_partial << std::endl;
  std::cout << "Avg Time to send the partial model: " << time_send_partial / num_to_find_partial << std::endl;
  std::cout << "Avg Time to assign slice id: " << time_assign_slice_id / num_to_find_partial << std::endl;
  std::cout << "----------------------------------------------------------\n";
  std::cout << "global model count: " << lda_global_vars->counts << std::endl;
  std::cout << "Avg Time (find_partial) function: " << lda_global_vars->time_whole / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of actual finding: " << lda_global_vars->time_find_partial / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of compression: " << lda_global_vars->time_compress / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of init: " << lda_global_vars->time_temp / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of copying ttemp: " << lda_global_vars->time_ttemp / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of serializing whole nvt: " << lda_global_vars->time_nvt_find / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of checking sparsity: " << lda_global_vars->time_check_sparse / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of serializing the sparse: " << lda_global_vars->time_serial_sparse / lda_global_vars->counts << std::endl;
  std::cout << "\tAvg Time of compressing: " << lda_global_vars->time_compress / lda_global_vars->counts << std::endl;

  tokens_sampled = 0;
  send_size = 0;
  receive_size = 0;
  start_time_tokens = get_time_ms();
}
void PSSparseServerTask::pre_assign_slices(int slice_size) {

  num_slices = lda_global_vars->pre_assign_slices(slice_size);

  unused_slice_id.reserve(num_slices);
  for (int i = 0; i < num_slices; ++i) {
    unused_slice_id.push_back(i);
  }

  std::cout << "Finish pre-assigning slcies" << std::endl;
}

void PSSparseServerTask::destroy_pthread_barrier(pthread_barrier_t* barrier) {
  // this fails if barrier has been allocated but not initialized
  // we don't handle this situation
  if (pthread_barrier_destroy(barrier) != 0) {
    throw std::runtime_error("Error destroying barrier");
  }
  delete barrier;
}

} // namespace cirrus
