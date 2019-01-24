#include "SDCA.h"
#include "SparseLRSDCAModel.h"

namespace cirrus {

SDCA::SDCA(double lr) : OptimizationMethod(lr) {}

void SDCA::sdca_update(std::unique_ptr<SparseLRSDCAModel>& lr_model,
                       const ModelGradient* gradient) {
  const LRSDCASparseGradient* grad =
      dynamic_cast<const LRSDCASparseGradient*>(gradient);

  if (grad == nullptr) {
    throw std::runtime_error("Error in dynamic cast");
  }

  for (const auto& w : grad->w) {
    int index = w.first;
    FEATURE_TYPE value = w.second;
    lr_model->weights_[index] +=
        value / (learning_rate * lr_model->dual_size());
  }

  for (const auto& a : grad->a) {
    int index = a.first;
    FEATURE_TYPE value = a.second;
    lr_model->coord_weights_[index] += value;
  }
}

void SDCA::sgd_update(std::unique_ptr<cirrus::SparseLRModel>& lr_model,
                      const cirrus::ModelGradient* gradient) {
  throw std::runtime_error("Not Implemented");
}

}  // namespace cirrus
