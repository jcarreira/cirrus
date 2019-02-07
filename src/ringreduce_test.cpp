#include "RingReduce.h"

#include <memory>
#include <vector>
#include <thread>
#include <functional>
#include <iostream>

void worker(int worker_id,
            const std::vector<std::pair<std::string, int>>& servers,
            const std::vector<float>& params) {
  std::cout << "worker: " << worker_id << std::endl;

  cirrus::RingReduce r(servers, worker_id);

  auto res = r.reduce(params);

  if (worker_id == 0) {
    std::cout << "worker 0< res.size" << res.size() << std::endl;
    for (const auto& v : res) {
      std::cout << v << " " << std::endl;
    }
  }
}

int main() {
  std::vector<float> params;
  for (int i = 0; i < 200; ++i) {
    params.push_back(i);
  }

  std::vector<std::pair<std::string, int>> servers;
  servers.push_back(std::make_pair("127.0.0.1", 7000));
  servers.push_back(std::make_pair("127.0.0.1", 7001));
  //servers.push_back(std::make_pair("127.0.0.1", 7002));
  
  std::shared_ptr<std::thread> t1 = std::make_shared<std::thread>(
      worker, 0, std::ref(servers), std::ref(params));
  std::shared_ptr<std::thread> t2 = std::make_shared<std::thread>(
      worker, 1, std::ref(servers), std::ref(params));
  //std::shared_ptr<std::thread> t3 = std::make_shared<std::thread>(
  //    worker, 2, std::ref(servers), std::ref(params));

  t1->join();
  t2->join();
  //t3->join();

  return 0;
}
