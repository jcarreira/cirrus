#ifndef _CREATE_TENSOR_MESSAGE_REPLY_H_
#define _CREATE_TENSOR_MESSAGE_REPLY_H_

#include <cstdint>

namespace cirrus {

class CreateTensorMessageReply {
  public:
    const char* getReturnData() const;
    uint32_t getReturnSize() const;
    bool getBool() const;

  private:
    struct {
      int return_value;
    } data;
};

} // namespace cirrus

#endif  // _CREATE_TENSOR_MESSAGE_REPLY_H_
