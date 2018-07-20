#include <CreateTensorMessageReply.h>

namespace cirrus {

const char* CreateTensorMessageReply::getReturnData() const {
  return reinterpret_cast<const char*>(&data);
}

uint32_t CreateTensorMessageReply::getReturnSize() const {
  return sizeof(data);
}

bool CreateTensorMessageReply::getBool() const {
  return data.return_value;
}

}  // namespace cirrus
