#ifndef _ADD_TENSOR_MESSAGE_H_
#define _ADD_TENSOR_MESSAGE_H_

#include <SparseTensor.h>
#include <string>
#include <memory>

namespace cirrus {

class AddTensorMessage {
  public:
    AddTensorMessage(const std::string& tensor_name, const SparseTensor1D&);
    AddTensorMessage(const std::string& tensor_name, const SparseTensor2D&);
    explicit AddTensorMessage(const char*);

    uint32_t getTotalDataSize() const;
    uint32_t getDataSize() const;

//    uint32_t getTensorSizeSize() const;
//    char* getTensorSizeData() const;
//    
//    uint32_t getTensorSize() const;
//    char* getTensorData() const;

  private:
    void populateData();

    std::string tensor_name;
    std::shared_ptr<char[]> data;

    std::vector<uint32_t> tensor_dim;
};

}  // namespace cirrus

#endif  // _ADD_TENSOR_MESSAGE_H_
