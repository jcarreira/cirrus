#include <AddTensorMessageReply.h>

namespace cirrus {

char* AddTensorMessageReply::getReturnData() {
  return reinterpret_cast<char*>(&msg);
}

uint32_t AddTensorMessageReply::getReturnSize() const {
  return sizeof(msg);
}

bool AddTensorMessageReply::getBool() const {
  return msg.return_value;
}

}  // namespace cirrus
