#include <GetSparseTensorMessage.h>
#include <Utils.h>

namespace cirrus {

  GetSparseTensorMessage::GetSparseTensorMessage(
      const std::string& name, const std::vector<uint32_t>& indexes) 
  : name(name), indexes(indexes)
{
  buildData();
}

char* GetSparseTensorMessage::getData() const {
  return msg_data.get();
}

uint32_t GetSparseTensorMessage::getDataSize() const {
  uint32_t msg_size =
    name.size() + 1 + sizeof(uint32_t) + indexes.size() * sizeof(uint32_t);
  return msg_size;
}

void GetSparseTensorMessage::buildData() {
  // message format
  // name (with nullptr)
  // num of indexes (uint32_t)
  // indexes
  msg_data.reset(new char[getDataSize()]);

  char* ptr = msg_data.get();
  std::copy(name.begin(), name.end(), msg_data.get());
  ptr += name.size();
  store_value<char>(ptr, 0);
  store_value<uint32_t>(ptr, indexes.size());
  std::copy(indexes.begin(), indexes.end(), reinterpret_cast<uint32_t*>(ptr));
}

}
