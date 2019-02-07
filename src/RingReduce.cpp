#include <RingReduce.h>

#include <iostream>
#include <cstring>
#include <thread>
#include <functional>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <Utils.h>

#include <string>
#include <vector>
#include <thread>
#include <cassert>
#include <memory>
#include <algorithm>
#include <atomic>
#include <map>
#include <iomanip>
#include <functional>


namespace cirrus {

// worker_id goes from 0 to #workers-1
RingReduce::RingReduce(
    const std::vector<std::pair<std::string, int>>& workers,
    int worker_id,
    const std::vector<float>& params) :
  workers(workers),
  worker_id(worker_id),
  params(params) {
    std::cout << "Starting ring reduce" << std::endl;

    for (const auto& v : workers) {
      std::cout << v.first << " " << v.second << std::endl;
    }
}

//void RingReduce::receive_new_connection() {
//  struct sockaddr_in cli_addr;
//  socklen_t clilen = sizeof(cli_addr);
//  int newsock = accept(server_sock_,
//      reinterpret_cast<struct sockaddr*> (&cli_addr),
//      &clilen);
//
//  uint32_t worker_id = 0;
//  if (read_all(newsock, &worker_id, sizeof(uint32_t)) == 0) {
//    throw std::string("Error in read_all");
//  }
//
//  worker_to_fd[worker_id] = newsock;
//}

// starts server, receives connections
void RingReduce::start_server(unsigned long int nworkers, int worker_id) {
  std::cout << "Starting server" << std::endl;
  std::cout << "nworkers: " << nworkers
    << " worker_id: " << worker_id
    << std::endl;

  int server_sock_ = socket(AF_INET, SOCK_STREAM, 0);
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

  int port = 9300 + worker_id;
  serv_addr.sin_port = htons(port);
  std::memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

  std::cout << "Doing bind" << std::endl;
  int ret = bind(server_sock_,
      reinterpret_cast<sockaddr*> (&serv_addr), sizeof(serv_addr));
  if (ret < 0) {
    throw std::runtime_error("Error binding in port " + to_string(port));
  }

  std::cout << "Doing listen" << std::endl;
  if (listen(server_sock_, SOMAXCONN) == -1) {
    throw std::runtime_error("Error listening on port " + to_string(port));
  }

  // receive neighbor connection
  struct sockaddr_in cli_addr;
  socklen_t clilen = sizeof(cli_addr);
  std::cout << "Doing accept" << std::endl;
  int newsock = accept(server_sock_,
      reinterpret_cast<struct sockaddr*> (&cli_addr),
      &clilen);

  neighbor_receive = newsock;

  sleep(1000);
}

void RingReduce::connect_to_neighbor(std::string ip, int port) {
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw std::runtime_error("Error when creating socket.");
  }
  int opt = 1;
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt))) {
    throw std::runtime_error("Error setting socket options.");
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) != 1) {
    throw std::runtime_error("Address family invalid or invalid "
        "IP address passed in");
  }
  // Save the port in the info
  serv_addr.sin_port = htons(port);
  std::memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

  int ret = ::connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  if (ret < 0) {
    throw std::runtime_error("Failed to make contact with server with ip: " +
        ip + " port: " + std::to_string(port) + "\n");
  }

  neighbor_receive = ret;

  std::cout << "Connected" << std::endl;
}

void RingReduce::send_to_neighbor(int neighbor_id) {
  // send all the floats

  std::cout << "Sending data to neighbor " << neighbor_id << std::endl;

  int left_index = (params.size() / workers.size()) * worker_id;
  int right_index = left_index + params.size() / workers.size(); // exclusive
  
  std::cout << "left_index: " << left_index << " right_index: " << right_index << std::endl;

  // last worker might be assigned more data
  if (worker_id == workers.size() - 1) {
    right_index = params.size();
  }

  int size = (right_index - left_index) * sizeof(float);
  const float* data = static_cast<const float*>(params.data());
  int ret = send_all(neighbor_send,
                     const_cast<void*>(reinterpret_cast<const void*>(data + left_index)),
                     size);
  if (ret != size) {
    throw std::string("send error");
  }
}

void RingReduce::receive_from_neighbor() {
  char data[10000];
  int size = 100 * sizeof(float);

  std::cout << "Receiving from neighbor " << std::endl;
  int ret = read_all(neighbor_receive, data, size);
  if (ret != size) {
    throw std::string("Error in read_all");
  }
}


// we do ring reduce here
std::vector<float> RingReduce::reduce(const std::vector<float>& params) {
  
  // starts server that waits for neighbor connection
  std::shared_ptr<std::thread> t = std::make_shared<std::thread>(
      std::bind(&RingReduce::start_server, this,
        std::placeholders::_1, std::placeholders::_2),
        workers.size(), worker_id);
  
  //start_server(workers.size(), worker_id);

  // connect to worker
  int neighbor_send_id = (worker_id + 1) % workers.size();
  int neighbor_receive = worker_id == 0 ? workers.size() - 1 : worker_id - 1;
  connect_to_neighbor(
      workers[neighbor_send_id].first,
      workers[neighbor_send_id].second);

  // #workers rounds
  int reduce_rounds = workers.size() - 1;
  //// scatter reduce
  for (int i = 0; i < reduce_rounds; ++i) {
    // worker 0 sends block 0 to worker 1 and receives block N-1 from worker N-1

    // send block i to worker i + 1
    receive_mutex.lock();
    std::shared_ptr<std::thread> t = std::make_shared<std::thread>(
        std::bind(&RingReduce::receive_from_neighbor, this));

    send_to_neighbor(i);
    
    receive_mutex.lock();
  }
}

};

