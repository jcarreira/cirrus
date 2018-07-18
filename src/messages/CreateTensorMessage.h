#ifndef _CREATE_TENSOR_MESSAGE_H_
#define _CREATE_TENSOR_MESSAGE_H_

#include <string>
#include <vector>

namespace cirrus {

class CreateTensorMessage {
  public:
    CreateTensorMessage(const std::string& tensor_name, const std::vector<uint32_t>& tensor_dim);
    explicit CreateTensorMessage(const char* data);

    std::string get_name() const;
    std::vector<uint32_t> get_tensor_dims() const;

    uint32_t get_data_size() const;
    
    const char* get_data() const;

  private:
    std::string tensor_name;
    const std::vector<uint32_t> tensor_dim;
};

} // namespace cirrus

#endif  // _CREATE_TENSOR_MESSAGE_H_
