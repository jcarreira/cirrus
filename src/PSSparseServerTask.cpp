#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "Constants.h"
#include "Checksum.h"
#include <signal.h>

#undef DEBUG

#define MAX_CONNECTIONS (nworkers * 2 + 1) // (2 x # workers + 1)

namespace cirrus {
    
PSSparseServerTask::PSSparseServerTask(
    uint64_t MODEL_GRAD_SIZE, uint64_t MODEL_BASE,
    uint64_t LABEL_BASE, uint64_t GRADIENT_BASE,
    uint64_t SAMPLE_BASE, uint64_t START_BASE,
    uint64_t batch_size, uint64_t samples_per_batch,
    uint64_t features_per_sample, uint64_t nworkers,
    uint64_t worker_id) :
  MLTask(MODEL_GRAD_SIZE, MODEL_BASE,
      LABEL_BASE, GRADIENT_BASE, SAMPLE_BASE, START_BASE,
      batch_size, samples_per_batch, features_per_sample,
      nworkers, worker_id) {
  std::cout << "PSSparseServerTask is built" << std::endl;

  operation_to_name[0] = "SEND_LR_GRADIENT";
  operation_to_name[1] = "SEND_MF_GRADIENT";
  operation_to_name[2] = "GET_LR_FULL_MODEL";
  operation_to_name[3] = "GET_MF_FULL_MODEL";
  operation_to_name[4] = "GET_LR_SPARSE_MODEL";
  operation_to_name[5] = "GET_MF_SPARSE_MODEL";
  operation_to_name[6] = "SET_TASK_STATUS";
  operation_to_name[7] = "GET_TASK_STATUS";
}

std::shared_ptr<char> PSSparseServerTask::serialize_lr_model(
    const SparseLRModel& lr_model, uint64_t* model_size) const {
  *model_size = lr_model.getSerializedSize();
  auto d = std::shared_ptr<char>(
      new char[*model_size], std::default_delete<char[]>());
  lr_model.serializeTo(d.get());
  return d;
}

bool PSSparseServerTask::testRemove(struct pollfd x) {
  // If this pollfd will be removed, the index of the next location to insert
  // should be reduced by one correspondingly.
  if (x.fd == -1) {
    curr_index -= 1;
  }
  return x.fd == -1;
}

bool PSSparseServerTask::process_send_mf_gradient(const Request& req, std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
#ifdef DEBUG 
  std::cout << "APPLY_GRADIENT_REQ incoming size: " << incoming_size << std::endl;
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
      
bool PSSparseServerTask::process_send_lr_gradient(const Request& req, std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
#ifdef DEBUG 
  std::cout << "APPLY_GRADIENT_REQ incoming size: " << incoming_size << std::endl;
#endif
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }
  //buffer.resize(incoming_size);
  try {
    if (read_all(req.sock, thread_buffer.data(), incoming_size) == 0) {
      return false;
    }
  } catch(...) {
    throw std::runtime_error("Uhandled error");
  }

  LRSparseGradient gradient(0);
  gradient.loadSerialized(thread_buffer.data());

  model_lock.lock();
  if (task_config.get_use_adagrad()) {
    lr_model->sgd_update_adagrad(
        task_config.get_learning_rate(), &gradient);
  } else {
    lr_model->sgd_update(
        task_config.get_learning_rate(), &gradient);
  }
  model_lock.unlock();
  gradientUpdatesCount++;
  return true;
}

// XXX we have to refactor this ASAP
// move this to SparseMFModel
bool PSSparseServerTask::process_get_mf_sparse_model(
    const Request& req, std::vector<char>& thread_buffer) {
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

#ifdef DEBUG
  std::cout << "k_items: " << k_items << std::endl;
  std::cout << "base_user_id: " << base_user_id << std::endl;
  std::cout << "minibatch_size: " << minibatch_size << std::endl;
#endif

  SparseMFModel sparse_mf_model((uint64_t)0, 0, 0);
  std::vector<char> data_to_send = sparse_mf_model.serializeFromDense(
      *mf_model, base_user_id, minibatch_size, k_items, thread_buffer.data());

  uint32_t to_send_size = data_to_send.size();
  if (send_all(req.sock, &to_send_size, sizeof(uint32_t)) == -1) {
    return false;
  }
  if (send_all(req.sock, data_to_send.data(), data_to_send.size()) == -1) {
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
  } catch(...) {
    throw std::runtime_error("Uhandled error");
  }

  const char* data = thread_buffer.data();
  uint64_t num_entries = load_value<uint32_t>(data);

  uint32_t to_send_size = num_entries * sizeof(FEATURE_TYPE);
  assert(to_send_size < 1024 * 1024);
  char data_to_send[1024*1024]; // 1MB
  char* data_to_send_ptr = data_to_send;
#ifdef DEBUG
  std::cout << "Sending back: " << num_entries
    << " weights from model. Size: " << to_send_size
    << std::endl;
#endif
  for (uint32_t i = 0; i < num_entries; ++i) {
    uint32_t entry_index = load_value<uint32_t>(data);
    store_value<FEATURE_TYPE>(
        data_to_send_ptr,
        lr_model->get_nth_weight(entry_index));
  }
  if (send_all(req.sock, data_to_send, to_send_size) == -1) {
    return false;
  }
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
  uint32_t model_size = lr_model_copy.getSerializedSize();
  
  if (thread_buffer.size() < model_size) {
    std::string error_str = "buffer with size " + std::to_string(thread_buffer.size()) +
        "too small: " + std::to_string(model_size);
    throw std::runtime_error(error_str);
  }

  lr_model_copy.serializeTo(thread_buffer.data());
  if (send_all(req.sock, thread_buffer.data(), model_size) == -1)
    return false;
  return true;
}

void PSSparseServerTask::gradient_f() {
  std::vector<char> thread_buffer;
  thread_buffer.resize(30 * 1024 * 1024); // 30 MB
  while (1) {
    sem_wait(&sem_new_req);
    to_process_lock.lock();
    Request req = std::move(to_process.front());
    to_process.pop();
    to_process_lock.unlock();

#ifdef DEBUG
    std::cout << "Processing request: " << req.req_id << std::endl;
#endif

    if (req.req_id == SEND_LR_GRADIENT) {
      if (!process_send_lr_gradient(req, thread_buffer)) {
        break;
      }
    }  else if (req.req_id == SEND_MF_GRADIENT) {
      if (!process_send_mf_gradient(req, thread_buffer)) {
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
    } else if (req.req_id == GET_MF_SPARSE_MODEL) {
      std::cout << "process_get_mf_sparse_model" << std::endl;
      if (!process_get_mf_sparse_model(req, thread_buffer)) {
        break;
      }
    } else if (req.req_id == GET_LR_FULL_MODEL) {
      if (!process_get_lr_full_model(req, thread_buffer))
        break;
    } else if (req.req_id == GET_MF_FULL_MODEL) {
      if (!process_get_mf_full_model(req, thread_buffer))
        break;
    } else {
      throw std::runtime_error("gradient_f: Unknown operation");
    }
    
    // We reactivate events from the client socket here
    req.poll_fd.events = POLLIN;
    //pthread_kill(main_thread, SIGUSR1);
    assert(write(pipefd[1], "a", 1) == 1); // wake up poll()
#ifdef DEBUG
    std::cout << "gradient_f done" << std::endl;
#endif
  }
}

/**
  * FORMAT
  * operation (uint32_t)
  * incoming size (uint32_t)
  * buffer with previous size
  */
bool PSSparseServerTask::process(struct pollfd& poll_fd) {
  int sock = poll_fd.fd;
#ifdef DEBUG
  std::cout << "Processing socket: " <<  sock << std::endl;
#endif
  uint32_t operation = 0;
  if (read_all(sock, &operation, sizeof(uint32_t)) == 0) { // read operation
    return false;
  }
#ifdef DEBUG 
  std::cout << "Operation: " << operation << " - " << operation_to_name[operation] << std::endl;
#endif

  if (operation == SEND_LR_GRADIENT || operation == SEND_MF_GRADIENT ||
      operation == GET_LR_SPARSE_MODEL || operation == GET_MF_SPARSE_MODEL) {
    uint32_t incoming_size = 0;
    if (read_all(sock, &incoming_size, sizeof(uint32_t)) == 0) {
      return false;
    }
    //data_ptr = buffer.data();
#ifdef DEBUG 
    std::cout << "incoming size: " << incoming_size << std::endl;
#endif
    to_process_lock.lock();
    poll_fd.events = 0; // explain this
    to_process.push(Request(operation, sock, incoming_size, poll_fd));
    to_process_lock.unlock();
    sem_post(&sem_new_req);
  } else if (operation == GET_LR_FULL_MODEL || operation == GET_MF_FULL_MODEL) {
    to_process_lock.lock();
    poll_fd.events = 0; // we disable events for this socket while we process this message
    to_process.push(Request(operation, sock, 0, poll_fd));
    to_process_lock.unlock();
    sem_post(&sem_new_req);
  } else if (operation == GET_TASK_STATUS) {
    uint32_t task_id;
    if (read_all(sock, &task_id, sizeof(uint32_t)) == 0) {
      return false;
    }
#ifdef DEBUG
    std::cout << "Get status task id: " << task_id << std::endl;
#endif
    assert(task_id < 10000);
    if (task_to_status.find(task_id) == task_to_status.end() ||
        task_to_status[task_id] == false) {
      uint32_t status = 0;
      send_all(sock, &status, sizeof(uint32_t));
    } else {
      uint32_t status = 1;
      send_all(sock, &status, sizeof(uint32_t));
    }
  } else if (operation == SET_TASK_STATUS) {
    uint32_t data[2] = {0}; // id + status
    if (read_all(sock, data, sizeof(uint32_t) * 2) == 0) {
      return false;
    }
#ifdef DEBUG
    std::cout << "Set status task id: " << data[0] << " status: " << data[1] << std::endl;
#endif
    task_to_status[data[0]] = data[1];
  } else {
    std::string error = "process: Uknown operation " + std::to_string(operation);
    throw std::runtime_error(error);
  }
  return true;
}

void PSSparseServerTask::start_server() {
  lr_model.reset(new SparseLRModel(MODEL_GRAD_SIZE));
  lr_model->randomize();
  mf_model.reset(new MFModel(task_config.get_users(), task_config.get_items(), NUM_FACTORS));
  mf_model->randomize();
  
  sem_init(&sem_new_req, 0, 0);

  server_thread = std::make_unique<std::thread>(
      std::bind(&PSSparseServerTask::poll_thread_fn, this));

  for (uint32_t i = 0; i < n_threads; ++i) {
    gradient_thread.push_back(std::make_unique<std::thread>(
        std::bind(&PSSparseServerTask::gradient_f, this)));
  }
}

void PSSparseServerTask::poll_thread_fn() {
  std::cout << "Starting server2" << std::endl;

  poll_thread = pthread_self();

  server_sock_ = socket(AF_INET, SOCK_STREAM, 0); 
  if (server_sock_ < 0) {
    throw std::string("Server error creating socket");
  }   

  int opt = 1;
  if (setsockopt(server_sock_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt))) {
    throw std::runtime_error("Error setting socket options.");
  }   
  if (setsockopt(server_sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    throw std::runtime_error("Error forcing port binding");
  }

  if (setsockopt(server_sock_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
    throw std::runtime_error("Error forcing port binding");
  }   

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_);
  std::memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

  int ret = bind(server_sock_, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr));
  if (ret < 0) {
    throw std::runtime_error("Error binding in port " + to_string(port_));
  }

  if (listen(server_sock_, SOMAXCONN) == -1) {
    throw std::runtime_error("Error listening on port " + to_string(port_));
  }
  fds.at(curr_index).fd = server_sock_;
  fds.at(curr_index).events = POLLIN;
  curr_index++;
  fds.at(curr_index).fd = pipefd[0];
  fds.at(curr_index).events = POLLIN;
  curr_index++;
  loop();
}

void PSSparseServerTask::loop() {
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);

  buffer.resize(10 * 1024 * 1024); // reserve 10MB upfront

  std::cout << "Starting loop" << std::endl;
  while (1) {
    int poll_status = poll(fds.data(), curr_index, timeout);
    if (poll_status == -1) {
      if (errno != EINTR) {
        throw std::runtime_error("Server error calling poll.");
      } else {
        std::cout << "EINTR" << std::endl;
      }
    } else if (fds[1].revents == POLLIN) {
      //std::cout << "Ignoring" << std::endl;
      fds[1].revents = 0;  // Reset the event flags
      char a[1];
      assert(read(pipefd[0], a, 1) >= 0);
      // ignore
    } else if (poll_status == 0) {
      //std::cout << timeout << " ms elapsed" << std::endl;
    } else {
      // there is at least one pending event, find it.
      for (uint64_t i = 0; i < curr_index; i++) {
	struct pollfd& curr_fd = fds[i];
	// Ignore the fd if we've said we don't care about it
	if (curr_fd.fd == -1) {
	  continue;
	}
	if (curr_fd.revents != POLLIN) {
	  //LOG<ERROR>("Non read event on socket: ", curr_fd.fd);
	  if (curr_fd.revents & POLLHUP) {
            std::cout << "PS closing connection " << num_connections << std::endl;
            num_connections--;
	    close(curr_fd.fd);
	    curr_fd.fd = -1;
	  }
	} else if (curr_fd.fd == server_sock_) {
          std::cout << "PS new connection!" << std::endl;
	  int newsock = accept(server_sock_,
	      reinterpret_cast<struct sockaddr*>(&cli_addr),
	      &clilen);
	  if (newsock < 0) {
	    throw std::runtime_error("Error accepting socket");
	  }
	  // If at capacity, reject connection
          if (num_connections > (MAX_CONNECTIONS - 1)) {
            std::cout << "Rejecting connection "
              << num_connections
              << std::endl;
            close(newsock);
          } else if (curr_index == max_fds) {
            throw std::runtime_error("We reached capacity");
	    close(newsock);
	  } else {
	    fds[curr_index].fd = newsock;
	    fds[curr_index].events = POLLIN;
	    curr_index++;
            num_connections++;
	  }
	} else {
#ifdef DEBUG
          std::cout << "Calling process" << std::endl;
#endif
	  if (!process(curr_fd)) {
            if (close(curr_fd.fd) != 0) {
              std::cout << "Error closing socket. errno: " << errno << std::endl;
            }
            num_connections--;
            std::cout << "PS closing connection after process(): " << num_connections << std::endl;
	    curr_fd.fd = -1;
	  }
	}
	curr_fd.revents = 0;  // Reset the event flags
      }
    }
    // If at max capacity, try to make room
    if (curr_index == max_fds) {
      // Try to purge unused fds, those with fd == -1
      std::cout << "Purging" << std::endl;
      std::remove_if(fds.begin(), fds.end(),
          std::bind(&PSSparseServerTask::testRemove, this, std::placeholders::_1));
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
    << " MODEL_GRAD_SIZE: " << MODEL_GRAD_SIZE
    << std::endl;

  assert(pipe(pipefd) != -1);

  main_thread = pthread_self();
  if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
    throw std::runtime_error("Unable to set signal handler");
  }
  
  task_config = config;
  start_server();

  //wait_for_start(PS_SPARSE_SERVER_TASK_RANK, redis_con, nworkers);

  uint64_t start = get_time_us();
  uint64_t last_tick = get_time_us();
  while (1) {
    auto now = get_time_us();
    auto elapsed_us = now - last_tick;
    auto since_start_sec = 1.0 * (now - start) / 1000000;
    if (elapsed_us > 1000000) {
      last_tick = now;
      std::cout << "Events in the last sec: " 
        << 1.0 * gradientUpdatesCount / elapsed_us * 1000 * 1000
        << " since (sec): " << since_start_sec
        << " #conns: " << num_connections
        << std::endl;
      gradientUpdatesCount = 0;
    }
    sleep(1);
  }
}

void PSSparseServerTask::checkpoint_model() const {
  uint64_t model_size;
  std::shared_ptr<char> data = serialize_lr_model(*lr_model, &model_size);

  std::ofstream fout("model_backup_file", std::ofstream::binary);
  fout.write(data.get(), model_size);
  fout.close();
}

} // namespace cirrus
