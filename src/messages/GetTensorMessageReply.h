#ifndef _GET_TENSOR_MESSAGE_REPLY_H_
#define _GET_TENSOR_MESSAGE_REPLY_H_

#include <string>
#include <vector>

namespace cirrus {

class GetTensorMessageReply {
  public:

    char* getTensorSizeData() const;
    void reserveTensorData();
    char* getTensorData() const;
    uint32_t getTensorSize() const;

  private:
};

} // namespace cirrus

#endif  // _GET_TENSOR_MESSAGE_REPLY_H_
