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
#include <SoftmaxModel.h>
#include <Tasks.h>
#include "SGD.h"
#include "Serializers.h"
#include "Utils.h"

using namespace cirrus;

cirrus::Configuration config =
    cirrus::Configuration("configs/softmax_config.cfg");

int main() {
  InputReader input;
  Dataset train_dataset = input.read_input_csv(
      "tests/test_data/train_mnist.csv", ",", 10, config.get_limit_samples(),
      config.get_limit_cols(), true);  // normalize=true

  std::unique_ptr<PSSparseServerInterface> psi =
      std::make_unique<PSSparseServerInterface>("127.0.0.1", 1337);
  psi->connect();
  int version = 0;
  while (1) {
    Dataset minibatch = train_dataset.random_sample(20);
    SoftmaxModel model = *(psi->get_sm_full_model(config));
    auto gradient = model.minibatch_grad(
        minibatch.get_samples(),
        const_cast<float*>(reinterpret_cast<const float*>(minibatch.get_labels().get())), 20,
        config.get_learning_rate());
    gradient->setVersion(version++);
    SoftmaxGradient* smg = dynamic_cast<SoftmaxGradient*>(gradient.get());
    psi->send_sm_gradient(*smg);
  }
}
