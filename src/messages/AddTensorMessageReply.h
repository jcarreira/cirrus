#ifndef _ADD_TENSOR_MESSAGE_REPLY_H_
#define _ADD_TENSOR_MESSAGE_REPLY_H_

#include <string>
#include <vector>

namespace cirrus {

class AddTensorMessageReply {
  public:
    char* getReturnData();
    uint32_t getReturnSize() const;
    bool getBool() const;

  private:
    struct {
      int return_value;
    } msg;
};

} // namespace cirrus

#endif  // _ADD_TENSOR_MESSAGE_REPLY_H_
