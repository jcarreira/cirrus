#include "OptimizationMethod.h"

namespace cirrus {

OptimizationMethod::OptimizationMethod(double lr)
  : learning_rate(lr)
{}
   
void OptimizationMethod::editWeight(double& weight) {
  return;
}

}  // namespace cirrus
