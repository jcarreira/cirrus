#include <CreateTensorMessage.h>
#include <Constants.h>
#include <Utils.h>

namespace cirrus {

/**
  * Format:
  * operation id (uint32_t)
  * message size
  * tensor_name with null ptr (tensor_name.size() + 1)
  * num_dims (uint32_t)
  * dims (each uint32_t)
  */

CreateTensorMessage::CreateTensorMessage(
    const std::string& tensor_name,
    uint32_t tensor_dim) :
  tensor_name(tensor_name),
  tensor_dim(std::vector<uint32_t>{tensor_dim}) {
    data.reset(new char[getTotalDataSize()]);
    populateData();
}

CreateTensorMessage::CreateTensorMessage(
    const std::string& tensor_name,
    const std::vector<uint32_t>& tensor_dim) :
  tensor_name(tensor_name),
  tensor_dim(tensor_dim) {
    data.reset(new char[getTotalDataSize()]);
    populateData();
}

CreateTensorMessage::CreateTensorMessage(const char* data) {
  throw "not implemented";
}

void CreateTensorMessage::populateData() {
  char* data_ptr = data.get();
  store_value<uint32_t>(data_ptr, CREATE_TENSOR_MSG);
  store_value<uint32_t>(data_ptr, getDataSize());
  store_value<std::string>(data_ptr, tensor_name);
  store_value(data_ptr, static_cast<char>(0)); // null after name
  store_value(data_ptr, tensor_dim.size());
  store_value(data_ptr, tensor_dim);
}
    
std::string CreateTensorMessage::getName() const {
  return tensor_name;
}
    
std::vector<uint32_t> CreateTensorMessage::getTensorDims() const {
  return tensor_dim;
}

uint32_t CreateTensorMessage::getDataSize() const {
  // all but the operation id
  return getTotalDataSize() - sizeof(uint32_t);
}
    
uint32_t CreateTensorMessage::getTotalDataSize() const {
  return
    sizeof(uint32_t) + // operation id
    tensor_name.size() + 1 + // tensor name with null
    sizeof(uint32_t) + // num dimensions
    tensor_dim.size(); // dimensions
}
    
const char* CreateTensorMessage::getData() const {
  return data.get();
}

}  // namespace cirrus
