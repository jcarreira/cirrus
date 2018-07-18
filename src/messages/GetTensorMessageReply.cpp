#include <GetTensorMessageReply.h>

namespace cirrus {

char* GetTensorMessageReply::get_tensor_size_data() const {
  throw "fix";
  return nullptr;
}

void GetTensorMessageReply::reserve_tensor_data() {
  throw "fix";
}

char* GetTensorMessageReply::get_tensor_data() const {
  throw "fix";
  return nullptr;
}

uint32_t GetTensorMessageReply::get_tensor_size() const {
  throw "fix";
  return 0;
}

}
