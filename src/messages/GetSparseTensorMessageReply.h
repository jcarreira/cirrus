#ifndef _GET_SPARSE_TENSOR_MESSAGE_REPLY_H_
#define _GET_SPARSE_TENSOR_MESSAGE_REPLY_H_

#include <string>
#include <vector>
#include <memory>

namespace cirrus {

class GetSparseTensorMessageReply {
  public:
    GetSparseTensorMessageReply(const std::vector<uint32_t>&);

    char* getData() const;
    void reserveData(const std::vector<uint32_t>& indexes);
    //char* get_tensor_data() const;
    uint32_t getSize() const;

  private:
    const std::vector<uint32_t>& indexes;

    std::shared_ptr<char[]> data;
};

} // namespace cirrus

#endif  // _GET_SPARSE_TENSOR_MESSAGE_REPLY_H_
