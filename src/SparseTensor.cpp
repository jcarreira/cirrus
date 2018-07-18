#include <SparseTensor.h>

namespace cirrus {

SparseTensor::SparseTensor(const char *data) {
  throw "fix";
}

//SparseTensor::SparseTensor(std::vector<std::pair<int, FEATURE_TYPE>>& tens)
//  : tensor(std::move(tens)) {
//}
    
SparseTensor::SparseTensor(std::vector<std::pair<int, FEATURE_TYPE>>&& tens)
  : tensor(std::move(tens)) {
}

}  // namespace cirrus
