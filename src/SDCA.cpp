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

  std::cout << "model w size = " << lr_model->weights_.size() << std::endl;
  std::cout << "model a size = " << lr_model->coord_weights_.size() << std::endl;

  std::cout << "w gradient: ";
  for (const auto& w : grad->w) {
    int index = w.first;
    FEATURE_TYPE value = w.second;
    std::cout << "(" << w.first << "," << w.second << ")";
    lr_model->weights_[index] += value;
  }
  std::cout << std::endl;

  std::cout << "a gradient: ";
  for (const auto& a : grad->a) {
    int index = a.first;
    FEATURE_TYPE value = a.second;
    std::cout << "(" << a.first << "," << a.second << ")";
    lr_model->coord_weights_[index] += value;
  }
  std::cout << std::endl;
}

void SDCA::sgd_update(std::unique_ptr<cirrus::SparseLRModel>& lr_model,
                      const cirrus::ModelGradient* gradient) {
  throw std::runtime_error("Not Implemented");
}

}  // namespace cirrus
