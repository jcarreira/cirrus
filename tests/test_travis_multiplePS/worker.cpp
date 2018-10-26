#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <Configuration.h>
#include <InputReader.h>
#include <MultiplePSSparseServerInterface.h>
#include <SparseLRModel.h>
#include <Tasks.h>
#include "SGD.h"
#include "Serializers.h"
#include "Utils.h"

using namespace cirrus;

cirrus::Configuration config = cirrus::Configuration("configs/test_config.cfg");

int main() {
  InputReader input;
  SparseDataset train_dataset = input.read_input_criteo_kaggle_sparse(
      "tests/test_data/train_lr.csv", ",", config);  // normalize=true
  train_dataset.check();
  train_dataset.print_info();

  std::vector<std::string> ps_ips;
  std::vector<uint64_t> ps_ports;

  ps_ips.push_back("127.0.0.1");
  ps_ips.push_back("127.0.0.1");

  ps_ports.push_back(1337);
  ps_ports.push_back(1339);

  SparseLRModel model(1 << config.get_model_bits());
  MultiplePSSparseServerInterface psi(ps_ips, ps_ports);
  int version = 0;
  while (1) {
    SparseDataset minibatch = train_dataset.random_sample(20);
    psi.get_lr_sparse_model_inplace(minibatch, model, config);
    auto gradient = model.minibatch_grad_sparse(minibatch, config);
    gradient->setVersion(version++);
    LRSparseGradient* lrg = dynamic_cast<LRSparseGradient*>(gradient.get());
    if (lrg == nullptr) {
      throw std::runtime_error("Error in dynamic cast");
    }
    psi.send_lr_gradient(*lrg);
  }
}
