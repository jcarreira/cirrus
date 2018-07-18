#ifndef _TENSOR_H_
#define _TENSOR_H_

#include <cstdint>
#include <vector>

namespace cirrus {

class Tensor {
  public:
    Tensor();
    Tensor(const char*);
    explicit Tensor(const std::vector<uint32_t>& dims);
    explicit Tensor(const std::vector<uint32_t>&& dims);
  private:
};

} // namespace cirrus

#endif  // _TENSOR_H_
