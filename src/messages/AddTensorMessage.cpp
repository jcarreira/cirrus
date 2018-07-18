#include <AddTensorMessage.h>

namespace cirrus {
    
AddTensorMessage::AddTensorMessage(const std::string& tensor_name,
                                   const SparseTensor&) {
}

AddTensorMessage::AddTensorMessage(const char*) {

}
    
uint32_t AddTensorMessage::get_tensor_size_size() const {
  throw "fix";
  return 0;
}

uint32_t AddTensorMessage::get_tensor_size() const {
  throw "fix";
  return 0;
}

char* AddTensorMessage::get_tensor_size_data() const {
  throw "fix";
  return nullptr;
}

char* AddTensorMessage::get_tensor_data() const {
  throw "fix";
  return nullptr;
}

}  // namespace cirrus
