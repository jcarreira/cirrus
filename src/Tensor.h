#ifndef _TENSOR_H_
#define _TENSOR_H_

#include <config.h>
#include <CreateTensorMessage.h>
#include <cstdint>
#include <vector>

namespace cirrus {

class Tensor {
  public:
    Tensor() = default;
    //Tensor(const char*);
    //explicit Tensor(const std::vector<uint32_t>& dims);
    //explicit Tensor(const std::vector<uint32_t>&& dims);
    virtual ~Tensor() = default;

    virtual uint32_t getDims() const = 0;

    static Tensor* generateTensor(
        const CreateTensorMessage&);
  private:
};

class Tensor1D : public Tensor {
  public:
    Tensor1D() = default;
    explicit Tensor1D(const char*) {}
    explicit Tensor1D(uint32_t dims);

    virtual ~Tensor1D() = default;

    uint32_t getDims() const override;
  private:
    std::vector<FEATURE_TYPE> data;
};

} // namespace cirrus

#endif  // _TENSOR_H_
