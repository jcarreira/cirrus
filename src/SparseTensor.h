#ifndef _SPARSE_TENSOR_H_
#define _SPARSE_TENSOR_H_

#include <config.h>
#include <vector>

namespace cirrus {

class SparseTensor {
  public:
    SparseTensor(const char*);
    
    //SparseTensor(std::vector<int, FEATURE_TYPE>&);
    SparseTensor(std::vector<std::pair<int, FEATURE_TYPE>>&&);
  private:
    std::vector<std::pair<int, FEATURE_TYPE>> tensor;
};

}  // namespace cirrus


#endif  // _SPARSE_TENSOR_H_
