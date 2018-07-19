#include <AddTensorMessage.h>

namespace cirrus {
    
AddTensorMessage::AddTensorMessage(const std::string& tensor_name,
                                   const SparseTensor1D&) {
}

AddTensorMessage::AddTensorMessage(const std::string& tensor_name,
                                   const SparseTensor2D&) {
}

AddTensorMessage::AddTensorMessage(const char*) {

}
    
uint32_t AddTensorMessage::getTensorSizeSize() const {
  throw "fix";
  return 0;
}

uint32_t AddTensorMessage::getTensorSize() const {
  throw "fix";
  return 0;
}

char* AddTensorMessage::getTensorSizeData() const {
  throw "fix";
  return nullptr;
}

char* AddTensorMessage::getTensorData() const {
  throw "fix";
  return nullptr;
}

}  // namespace cirrus
