#ifndef _GET_TENSOR_MESSAGE_REPLY_H_
#define _GET_TENSOR_MESSAGE_REPLY_H_

#include <string>
#include <vector>

namespace cirrus {

class GetTensorMessageReply {
  public:

    char* get_tensor_size_data() const;
    void reserve_tensor_data();
    char* get_tensor_data() const;
    uint32_t get_tensor_size() const;

  private:
};

} // namespace cirrus

#endif  // _GET_TENSOR_MESSAGE_REPLY_H_
