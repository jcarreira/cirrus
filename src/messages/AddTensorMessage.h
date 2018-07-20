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

    char* getData() const;

  private:
    /**
      * Create message
      */
    void populateData();

    std::string tensor_name;           //< name of tensor
    std::vector<uint32_t> tensor_dim;  //< dimensions of tensor

    std::shared_ptr<char[]> data;  //< message data
};

}  // namespace cirrus

#endif  // _ADD_TENSOR_MESSAGE_H_
