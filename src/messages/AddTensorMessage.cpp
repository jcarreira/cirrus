#include <AddTensorMessage.h>
#include <Constants.h>
#include <Utils.h>

namespace cirrus {

/**
  * Format:
  * operation id (uint32_t)
  * message size (uint32_t)
  * tensor_name with null ptr (tensor_name.size() + 1)
  * dimension (uint32_t)
  * tensor_values (each FEATURE_TYPE)
  */
    
AddTensorMessage::AddTensorMessage(const std::string& tensor_name,
                                   const SparseTensor1D& tensor)
  : tensor_name(tensor_name),
    tensor_dim(std::vector<uint32_t>{tensor.getSize()}) {
  data.reset(new char[getTotalDataSize()]);
  populateData();
}

AddTensorMessage::AddTensorMessage(const std::string& tensor_name,
                                   const SparseTensor2D&) {
}

AddTensorMessage::AddTensorMessage(const char*) {

}

void AddTensorMessage::populateData() {
  char* data_ptr = data.get();
  store_value<uint32_t>(data_ptr, ADD_TENSOR_MSG);
  store_value<uint32_t>(data_ptr, getDataSize());
  store_value<std::string>(data_ptr, tensor_name);
  store_value(data_ptr, static_cast<char>(0)); // null after name
  store_value(data_ptr, tensor_dim.size());
  store_value(data_ptr, tensor_dim);
}
    
char* AddTensorMessage::getData() const {
  return data.get();
}

uint32_t AddTensorMessage::getDataSize() const {
  // all but the operation id
  return getTotalDataSize() - sizeof(uint32_t);
}

uint32_t AddTensorMessage::getTotalDataSize() const {
  return
    sizeof(uint32_t) + // operation id
    sizeof(uint32_t) + // message size
    tensor_name.size() + 1 + // tensor name with null
    sizeof(uint32_t) + // num dimensions
    tensor_dim.size(); // dimensions
}

}  // namespace cirrus
