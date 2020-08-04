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
#include <SparseMFModel.h>
#include <Tasks.h>
#include "SGD.h"
#include "Serializers.h"
#include "Utils.h"

using namespace cirrus;

cirrus::Configuration config = cirrus::Configuration("configs/jester.cfg");

int main() {
  InputReader input;
  int nusers, njokes;
  SparseDataset train_dataset = input.read_jester_ratings(
      "tests/test_data/jester_train.csv", &nusers, &njokes);
  nusers = config.get_users();
  njokes = config.get_items();
  train_dataset.check();
  train_dataset.print_info();
  int nfactors = 10;
  int batch_size = config.get_minibatch_size();

  std::vector<std::string> ps_ips{"127.0.0.1", "127.0.0.1"};

  std::vector<uint64_t> ps_ports{1338, 1340};

  MultiplePSSparseServerInterface psi(config, ps_ips, ps_ports);
  while (true) {
    try {
      psi.connect();
      break;
    } catch (const std::exception& exc) {
      std::cout << exc.what();
    }
  }

  int version = 0;

  for (int t = 0; t < 2; t++) {
    for (uint32_t i = 0; i < nusers; i += batch_size) {
      SparseMFModel model(nusers, njokes, nfactors);
      int actual_batch_size = batch_size;
      if (i + batch_size >= nusers) {
        break;
      }
      SparseDataset ds = train_dataset.sample_from(i, actual_batch_size);
      psi.get_mf_sparse_model_inplace(ds, model, config, i, actual_batch_size);
      auto gradient = model.minibatch_grad(ds, config, i);
      gradient->setVersion(version++);
      MFSparseGradient* mfg = dynamic_cast<MFSparseGradient*>(gradient.get());
      if (mfg == nullptr) {
        throw std::runtime_error("Error in dynamic cast");
      }
      psi.send_mf_gradient(*mfg);
    };
  }
}
