#include <Tensor.h>

namespace cirrus {

//Tensor::Tensor(const char* data) {
//  throw "fix";
//}

//Tensor::Tensor(const std::vector<uint32_t>& dims) {
//  throw "fix";
//}
//
//Tensor::Tensor(const std::vector<uint32_t>&& dims) {
//  throw "fix";
//}

    
Tensor* Tensor::generateTensor(
        const CreateTensorMessage& msg) {
  if (msg.getTensorDims().size() == 1) {
    return new Tensor1D(msg.getTensorDims()[0]);
  } else if (msg.getTensorDims().size() == 2) {
    throw "fix";
  } else {
    throw std::runtime_error("Dims >2 not supported");
  }
}

Tensor1D::Tensor1D(uint32_t dims) {
  data.resize(dims);
}

uint32_t Tensor1D::getDims() const {
  return 1;
}

}  // namespace cirrus

