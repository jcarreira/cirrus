#include <SparseTensor.h>

namespace cirrus {

SparseTensor1D::SparseTensor1D(const char *data) {
  throw "fix";
}

//SparseTensor::SparseTensor(std::vector<std::pair<int, FEATURE_TYPE>>& tens)
//  : tensor(std::move(tens)) {
//}
    
SparseTensor1D::SparseTensor1D(std::vector<std::pair<int, FEATURE_TYPE>>&& tens)
  : tensor(std::move(tens)) {
}

}  // namespace cirrus
