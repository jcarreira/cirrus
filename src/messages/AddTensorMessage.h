#ifndef _ADD_TENSOR_MESSAGE_H_
#define _ADD_TENSOR_MESSAGE_H_

#include <SparseTensor.h>
#include <string>

namespace cirrus {

class AddTensorMessage {
  public:
    AddTensorMessage(const std::string& tensor_name, const SparseTensor&);
    explicit AddTensorMessage(const char*);

    uint32_t get_tensor_size_size() const;
    char* get_tensor_size_data() const;
    
    uint32_t get_tensor_size() const;
    char* get_tensor_data() const;
  private:

};

}  // namespace cirrus

#endif  // _ADD_TENSOR_MESSAGE_H_
