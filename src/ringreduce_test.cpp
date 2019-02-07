#include "RingReduce.h"

#include <memory>
#include <vector>
#include <thread>
#include <functional>

void worker(int worker_id,
            std::vector<std::pair<std::string, int>> servers,
            std::vector<float> params) {
  cirrus::RingReduce r(servers, worker_id);

  r.reduce(params);
}

int main() {
  std::vector<float> params;
  for (int i = 0; i < 200; ++i) {
    params.push_back(i);
  }

  std::vector<std::pair<std::string, int>> servers;
  servers.push_back(std::make_pair("127.0.0.1", 7000));
  servers.push_back(std::make_pair("127.0.0.1", 7001));
  
  std::shared_ptr<std::thread> t1 = std::make_shared<std::thread>(
      worker, 0, servers, params);
  std::shared_ptr<std::thread> t2 = std::make_shared<std::thread>(
      worker, 1, servers, params);

  t1->join();
  t2->join();

  return 0;
}
