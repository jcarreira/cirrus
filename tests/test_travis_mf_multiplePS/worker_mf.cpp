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
#include <PSSparseServerInterface.h>
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
  train_dataset.check();
  train_dataset.print_info();
  int nfactors = 10;
  int batch_size = 200;

  SparseMFModel model(nusers, njokes, nfactors);

  std::vector<std::string> ps_ips;
  ps_ips.push_back("127.0.0.1");
  ps_ips.push_back("127.0.0.1");

  std::vector<uint64_t> ps_ports;
  ps_ports.push_back(1338);
  ps_ports.push_back(1340);

  MultiplePSSparseServerInterface> psi(ps_ips, ps_ports);
  
  int version = 0;
  while (1) {
    for (uint64_t i = 0; i < nusers; i += batch_size) {
      int actual_batch_size = batch_size;
      if (i + batch_size >= nusers) {
        actual_batch_size = nusers - i - 1;
      }
      SparseDataset ds = train_dataset.sample_from(i, actual_batch_size);
      model = psi.get_sparse_mf_model(ds, i, actual_batch_size);
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
