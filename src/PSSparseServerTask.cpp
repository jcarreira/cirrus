#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "Constants.h"
#include "Checksum.h"
#include "CreateTensorMessage.h"
#include "AddTensorMessage.h"
#include "GetTensorMessage.h"

#include <signal.h>
#include "OptimizationMethod.h"
#include "AdaGrad.h"
#include "Momentum.h"
#include "SGD.h"
#include "Nesterov.h"

#undef DEBUG

#define MAX_CONNECTIONS (nworkers * 2 + 1) // (2 x # workers + 1)
#define THREAD_MSG_BUFFER_SIZE 5000000

namespace cirrus {

PSSparseServerTask::PSSparseServerTask(
    uint64_t model_size,
    uint64_t batch_size, uint64_t samples_per_batch,
    uint64_t features_per_sample, uint64_t nworkers,
    uint64_t worker_id, const std::string& ps_ip,
    uint64_t ps_port) :
  MLTask(model_size,
      batch_size, samples_per_batch, features_per_sample,
      nworkers, worker_id, ps_ip, ps_port) {

  std::cout << "PSSparseServerTask is built" << std::endl;

  std::atomic_init(&gradientUpdatesCount, 0UL);
  std::atomic_init(&thread_count, 0);

  operation_to_name[CREATE_TENSOR_MSG] = "CREATE_TENSOR_MSG";
  operation_to_name[ADD_TENSOR_MSG] = "ADD_TENSOR_MSG";
  operation_to_name[GET_TENSOR_MSG] = "GET_TENSOR_MSG";
  operation_to_name[GET_SPARSE_TENSOR_MSG] = "GET_SPARSE_TENSOR_MSG";
  operation_to_name[SET_TASK_STATUS] = "SET_TASK_STATUS";
  operation_to_name[GET_TASK_STATUS] = "GET_TASK_STATUS";
  operation_to_name[REGISTER_TASK] = "REGISTER_TASK";
  operation_to_name[GET_NUM_CONNS] = "GET_NUM_CONNS";
  operation_to_name[GET_LAST_TIME_ERROR] = "GET_LAST_TIME_ERROR";

  for (int i = 0; i < NUM_PS_WORK_THREADS; i++) {
    thread_msg_buffer[i] =
        new char[THREAD_MSG_BUFFER_SIZE]; // per-thread buffer
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

bool PSSparseServerTask::process_add_tensor_msg(
    const Request& req,
    std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }

  try {
    if (read_all(req.sock, thread_buffer.data(), incoming_size) == 0) {
      return false;
    }
  } catch (...) {
    throw std::runtime_error("Uhandled error");
  }

  AddTensorMessage add_tensor_msg(thread_buffer.data());

  // XXX implement here

  try {
    bool t = true;
    if (send_all(req.sock, &t, sizeof(t)) == -1) {
      return false;
    }
  } catch (...) {
    throw std::runtime_error("Uhandled error");
  }

  return true;
}

bool PSSparseServerTask::process_get_tensor_msg(
    const Request& req,
    std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }

}

bool PSSparseServerTask::process_get_sparse_tensor_msg(
    const Request& req,
    std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }

}

bool PSSparseServerTask::process_create_tensor_msg(
    const Request& req,
    std::vector<char>& thread_buffer) {
  uint32_t incoming_size = req.incoming_size;
  if (incoming_size > thread_buffer.size()) {
    throw std::runtime_error("Not enough buffer");
  }

  try {
    if (read_all(req.sock, thread_buffer.data(), incoming_size) == 0) {
      return false;
    }
  } catch (...) {
    throw std::runtime_error("Uhandled error");
  }

  CreateTensorMessage create_tensor_msg(thread_buffer.data());

  // XXX random initialize (?)
  name_to_tensor[create_tensor_msg.get_name()] = Tensor(create_tensor_msg.get_tensor_dims());
  
  try {
    bool t = true;
    if (send_all(req.sock, &t, sizeof(t)) == -1) {
      return false;
    }
  } catch (...) {
    throw std::runtime_error("Uhandled error");
  }

  return true;
}

void PSSparseServerTask::handle_failed_read(struct pollfd* pfd) {
  if (close(pfd->fd) != 0) {
    std::cout << "Error closing socket. errno: " << errno << std::endl;
  }
  num_connections--;
  std::cout << "PS closing connection after process(): " << num_connections
            << std::endl;
  pfd->fd = -1;
  pfd->revents = 0;
}

void PSSparseServerTask::gradient_f() {
  std::vector<char> thread_buffer;
  thread_buffer.resize(120 * 1024 * 1024); // 120 MB

  int thread_number = thread_count++;
  while (1) {
    sem_wait(&sem_new_req);
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

    // read the incoming_size value from the client
    req.req_id = operation;

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
      }
      send_all(sock, &task_reg, sizeof(uint32_t));
      continue;
    } else if (operation == CREATE_TENSOR_MSG || operation == ADD_TENSOR_MSG ||
        GET_TENSOR_MSG || operation == GET_SPARSE_TENSOR_MSG) {
      uint32_t incoming_size = 0;
      if (read_all(sock, &incoming_size, sizeof(uint32_t)) == 0) {
        if (close(req.poll_fd.fd) != 0) {
          std::cout << "Error closing socket. errno: " << errno << std::endl;
        }
        num_connections--;
        std::cout << "PS closing connection after process(): " << num_connections << std::endl;
        req.poll_fd.fd = -1;
        req.poll_fd.revents = 0;
        continue;
      }
      req.incoming_size = incoming_size;
    } else {
      throw std::runtime_error("Unknown operation");
    }

#ifdef DEBUG
    std::cout << "Processing request: " << req.req_id << std::endl;
#endif

    if (req.req_id == GET_TASK_STATUS) {
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
    } else if (req.req_id = CREATE_TENSOR_MSG) {
      process_create_tensor_msg(req, thread_buffer);
    } else if (req.req_id = ADD_TENSOR_MSG) { 
      process_add_tensor_msg(req, thread_buffer);
    } else if (req.req_id = GET_TENSOR_MSG) { 
      process_get_tensor_msg(req, thread_buffer);
    } else if (req.req_id = GET_SPARSE_TENSOR_MSG) { 
      process_get_sparse_tensor_msg(req, thread_buffer);
    } else if (operation == GET_NUM_CONNS) {
      std::cout << "Retrieve num connections: " << num_connections << std::endl;
      if (send(sock, &num_connections, sizeof(uint32_t), 0) < 0) {
        throw std::runtime_error("Error sending number of connections");
      }
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
}

/**
 * FORMAT
 * operation (uint32_t)
 * incoming size (uint32_t)
 * buffer with previous size
 */
bool PSSparseServerTask::process(struct pollfd& poll_fd, int thread_id) {
  int sock = poll_fd.fd;
#ifdef DEBUG
  std::cout << "Processing socket: " << sock << std::endl;
#endif

  uint32_t operation = 0;
#ifdef DEBUG 
  std::cout << "Operation: " << operation << " - "
      << operation_to_name[operation] << std::endl;
#endif

  uint32_t incoming_size = 0;
#ifdef DEBUG 
  std::cout << "incoming size: " << incoming_size << std::endl;
#endif
  to_process_lock.lock();
  poll_fd.events = 0; // explain this
  to_process.push(Request(operation, sock, thread_id, incoming_size, poll_fd));
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

  sem_init(&sem_new_req, 0, 0);

  for (int i = 0; i < NUM_POLL_THREADS; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    server_threads.push_back(std::make_unique<std::thread>(
          std::bind(&PSSparseServerTask::main_poll_thread_fn, this, i)));
  }

  for (uint32_t i = 0; i < NUM_PS_WORK_THREADS; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    gradient_thread.push_back(std::make_unique<std::thread>(
          std::bind(&PSSparseServerTask::gradient_f, this)));
  }

  // start checkpoing thread
  if (task_config.get_checkpoint_frequency() > 0) {
      checkpoint_thread.push_back(std::make_unique<std::thread>(
                  std::bind(&PSSparseServerTask::checkpoint_model_loop, this)));
  }
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
  loop(poll_id);
}

void PSSparseServerTask::loop(int poll_id) {
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);

  buffer.resize(10 * 1024 * 1024); // reserve 10MB upfront

  std::cout << "Starting loop for id: " << poll_id << std::endl;
  while (1) {
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
            num_connections--;
            close(curr_fd.fd);
            curr_fd.fd = -1;
          }
        } else if (poll_id == 0 && curr_fd.fd == server_sock_) {
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
    curr_indexes[i] == 0;
    fdses[i].resize(max_fds);
  }

  main_thread = pthread_self();
  if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
    throw std::runtime_error("Unable to set signal handler");
  }

  task_config = config;

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

void PSSparseServerTask::checkpoint_model_loop() {
    if (task_config.get_checkpoint_frequency() == 0) {
        // checkpoint disabled
        return;
    }

    while (true) {
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

} // namespace cirrus
