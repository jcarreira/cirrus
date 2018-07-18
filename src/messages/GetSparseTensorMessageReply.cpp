#include <GetSparseTensorMessageReply.h>

namespace cirrus {

GetSparseTensorMessageReply::GetSparseTensorMessageReply(
    const std::vector<uint32_t>& indexes)
  : indexes(indexes)
{
  reserve_data(indexes);
}

void GetSparseTensorMessageReply::reserve_data(const std::vector<uint32_t>& indexes) {
  data.reset(new char[indexes.size() * sizeof(uint32_t)]);
}

char* GetSparseTensorMessageReply::get_data() const {
  return data.get();
}

uint32_t GetSparseTensorMessageReply::get_size() const {
  return indexes.size() * sizeof(uint32_t);
}

#if 0
char* GetSparseTensorMessageReply::get_tensor_data() const {
  throw "fix";
  return nullptr;
}

uint32_t GetSparseTensorMessageReply::get_tensor_size() const {
  throw "fix";
  return 0;
}
#endif

}
