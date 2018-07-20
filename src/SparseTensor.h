#ifndef _SPARSE_TENSOR_H_
#define _SPARSE_TENSOR_H_

#include <config.h>
#include <vector>
#include <cstdint>

namespace cirrus {

class SparseTensor1D {
  public:
    SparseTensor1D(const char*);
    
    //SparseTensor(std::vector<int, FEATURE_TYPE>&);
    SparseTensor1D(std::vector<std::pair<int, FEATURE_TYPE>>&&);

    uint32_t getSize() const { return tensor.size(); }
  private:
    std::vector<std::pair<int, FEATURE_TYPE>> tensor;
};

class SparseTensor2D {
  public:
    SparseTensor2D(const char*);

  private:
};

}  // namespace cirrus

#endif  // _SPARSE_TENSOR_H_
