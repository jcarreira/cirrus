#include "Nesterov.h"

namespace cirrus {

Nesterov::Nesterov(double lr, double mb)
  : OptimizationMethod(lr), momentum_beta(mb) {}

void Nesterov::sgdUpdate(
    std::unique_ptr<SparseLRModel>& lr_model, 
    const ModelGradient* gradient) {
  const LRSparseGradient* grad =
    dynamic_cast<const LRSparseGradient*>(gradient);

  if (grad == nullptr) {
    throw std::runtime_error("Error in dynamic cast");
  }

  for (const auto& w : grad->weights) {
    int index = w.first;
    FEATURE_TYPE value = w.second;
    if (momentum_avg == 0.0) {
      momentum_avg = value;
    } else {
      momentum_avg = momentum_beta * momentum_avg + (1.0 - momentum_beta)
        * learning_rate * value;
    }
    lr_model->weights_[index] +=  momentum_avg;
  }
}

void Nesterov::editWeight(double& weight) {
  weight += momentum_beta * momentum_avg;
}

}  // namespace cirrus
