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
#include <SparseLRModel.h>
#include <Tasks.h>
#include "SGD.h"
#include "Serializers.h"
#include "Utils.h"

using namespace cirrus;

cirrus::Configuration config =
    cirrus::Configuration("configs/criteo_kaggle_sdca.cfg");

int main() {
  InputReader input;
  SparseDataset train_dataset = input.read_input_criteo_kaggle_sparse(
      "tests/test_data/train_lr.csv", ",", config);  // normalize=true
  train_dataset.check();
  train_dataset.print_info();

  SparseLRSDCAModel model(1 << config.get_model_bits(),
                          config.get_limit_samples());
  int num_minibatches =
      config.get_limit_samples() / config.get_minibatch_size();
  std::unique_ptr<PSSparseServerInterface> psi =
      std::make_unique<PSSparseServerInterface>("127.0.0.1", 1337);
  psi->connect();
  int version = 0;
  while (1) {
    int index = (rand() % num_minibatches) * config.get_minibatch_size();
    SparseDataset minibatch =
        train_dataset.sample_from(index, config.get_minibatch_size());
    psi->get_lr_sdca_model_inplace(model, config);
    auto gradient = model.minibatch_grad_indexed(index, minibatch, config);
    gradient->setVersion(version++);
    LRSDCASparseGradient* lrg =
        dynamic_cast<LRSDCASparseGradient*>(gradient.get());
    if (lrg == nullptr) {
      throw std::runtime_error("Error in dynamic cast");
    }
    psi->send_lr_sdca_gradient(*lrg);
  }
  return 0;
}
