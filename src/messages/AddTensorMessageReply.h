#ifndef _ADD_TENSOR_MESSAGE_REPLY_H_
#define _ADD_TENSOR_MESSAGE_REPLY_H_

#include <string>
#include <vector>

namespace cirrus {

class AddTensorMessageReply {
  public:
    const char* getReturnData() const {
      throw "fix";
      return nullptr;
    }
    
    const char* getReturnFix() const {
      throw "fix";
      return 0;
    }
    
    uint32_t getReturnSize() const {
      throw "fix";
      return 0;
    }

    bool getBool() const {
      throw "fix";
      return false;
    }

  private:
};

} // namespace cirrus

#endif  // _ADD_TENSOR_MESSAGE_REPLY_H_
