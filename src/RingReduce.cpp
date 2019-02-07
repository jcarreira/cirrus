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
    int worker_id) :
  workers(workers),
  worker_id(worker_id) {
    std::cout << "Starting ring reduce" << std::endl;

    for (const auto& v : workers) {
      std::cout << v.first << " " << v.second << std::endl;
    }
}

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

  std::cout << "Doing listen worker_id: " << worker_id
            << " port: " << port << std::endl;
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
}

void RingReduce::connect_to_neighbor() {
  std::string ip = workers[neighbor_send_id].first;
  int port = 9300 + neighbor_send_id;

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

  neighbor_send = sock;

  std::cout << "Connected neighbor_send " << neighbor_send << std::endl;
}

//std::pair<int, int> RingReduce::calculate_send_data(
//    int id, const std::vector<float>& params, int iter) {
//  // this worker is going to send block id - iter
//  int block_to_be_received = 0;
//  if (iter > id) {
//    block_to_be_received = workers.size() - (iter - id);
//  } else {
//    block_to_be_received = id - iter;
//  }
//
//  int left_index = (params.size() / workers.size()) * id;
//  int right_index = left_index + params.size() / workers.size(); // exclusive
//  
//  // last worker might be assigned more data
//  if (id == workers.size() - 1) {
//    right_index = params.size();
//  }
//  
//  int size = (right_index - left_index);
//
//  return std::make_pair(left_index, size);
//}

std::pair<int, int> RingReduce::calculate_receive_data(
    int id, const std::vector<float>& params, int iter) {

  // neighbor is going to send block id - iter
  int block_to_be_received = 0;
  if (iter > id) {
    block_to_be_received = workers.size() - (iter - id);
  } else {
    block_to_be_received = id - iter;
  }

  int left_index = (params.size() / workers.size()) * block_to_be_received;
  int right_index = left_index + params.size() / workers.size(); // exclusive
  
  // last worker might be assigned more data
  if (block_to_be_received == workers.size() - 1) {
    right_index = params.size();
  }
  
  int size = (right_index - left_index);

  return std::make_pair(left_index, size);
}

void RingReduce::send_to_neighbor(const std::vector<float>& params, int iter) {
  // send all the floats

  std::cout << "Sending data to neighbor " << neighbor_send_id << std::endl;

  auto data_size = calculate_receive_data(worker_id, params, iter);
  //auto data_size = calculate_send_data(worker_id, params, iter);
  
  int size = data_size.second * sizeof(float);
  
  std::cout << "left_index: " << data_size.first
            << " size: " << size
            << std::endl;

  const float* data = static_cast<const float*>(result.data());
  int ret = send_all(neighbor_send,
                     const_cast<void*>(
                       reinterpret_cast<const void*>(data + data_size.first)),
                     size);
  std::cout << "send_all ret: " << ret << std::endl;
  if (ret != size) {
    throw std::string("send error");
  }
}

void RingReduce::receive_from_neighbor(const std::vector<float>& params, int iter) {
  // we can calculate how much data we will receive
  auto data_size = calculate_receive_data(neighbor_receive_id, params, iter);
  std::shared_ptr<float[]> data =
    std::shared_ptr<float[]>(new float[data_size.second]);

  int size = data_size.second * sizeof(float);
  std::cout << "Receiving from neighbor " << std::endl;
  int ret = read_all(neighbor_receive, data.get(), size);
  if (ret != size) {
    //std::cout << "Error in read_all" << std::endl;
    throw std::string("Error in read_all");
  }

  std::cout << "Read from neighbour bytes: " << ret << std::endl;
  std::cout << "Summing left_index : " << data_size.first << std::endl;

  if (worker_id == 0) {
    for (int i = 0; i < 10; ++i) {
      std::cout << data.get()[i] << " ";
    }
    std::cout << std::endl;
  }

  // now we sum
  for (int i = 0; i < data_size.second; ++i) {
    result[data_size.first + i] += data.get()[i];
  }

  receive_mutex.unlock();
}

void RingReduce::test() {}

// we do ring reduce here
std::vector<float> RingReduce::reduce(const std::vector<float>& params) {

  result = params; // first we copy
  
  // starts server that waits for neighbor connection
  std::shared_ptr<std::thread> t = std::make_shared<std::thread>(
      std::bind(&RingReduce::start_server, this,
        std::placeholders::_1, std::placeholders::_2),
        workers.size(), worker_id);

  sleep(2);
  
  //start_server(workers.size(), worker_id);

  // connect to worker
  neighbor_send_id = (worker_id + 1) % workers.size();
  neighbor_receive_id = worker_id == 0 ? workers.size() - 1 : worker_id - 1;
  connect_to_neighbor();

  t->join();

  // #workers rounds
  int reduce_rounds = workers.size() - 1;
  //// scatter reduce
  for (int i = 0; i < reduce_rounds; ++i) {
    std::cout << "Round: " << i << std::endl;
    // worker w sends block w - i

    // send block i to worker i + 1
    receive_mutex.lock();
    std::shared_ptr<std::thread> t = std::make_shared<std::thread>(
        std::bind(&RingReduce::receive_from_neighbor, this,
          std::placeholders::_1, std::placeholders::_2),
        std::ref(params), i);

    send_to_neighbor(params, i);
    
    receive_mutex.lock();
    receive_mutex.unlock();

    t->join();
  }
  
  //for (int i = 0; i < reduce_rounds; ++i) {
  //  std::cout << "Round: " << i << std::endl;
  //  // worker w sends block w - i

  //  // send block i to worker i + 1
  //  receive_mutex.lock();
  //  std::shared_ptr<std::thread> t = std::make_shared<std::thread>(
  //      std::bind(&RingReduce::receive_from_neighbor, this,
  //        std::placeholders::_1, std::placeholders::_2),
  //      std::ref(params), i);

  //  send_to_neighbor(params, i);
  //  
  //  receive_mutex.lock();
  //  receive_mutex.unlock();

  //  t->join();
  //}

  return result;
}

};

