#ifndef _ADD_TENSOR_MESSAGE_REPLY_H_
#define _ADD_TENSOR_MESSAGE_REPLY_H_

#include <string>
#include <vector>

namespace cirrus {

class AddTensorMessageReply {
  public:
    const char* get_return_data() const {
      throw "fix";
      return nullptr;
    }
    
    const char* get_return_fix() const {
      throw "fix";
      return 0;
    }
    
    uint32_t get_return_size() const {
      throw "fix";
      return 0;
    }

    bool get_bool() const {
      throw "fix";
      return false;
    }

  private:
};

} // namespace cirrus

#endif  // _ADD_TENSOR_MESSAGE_REPLY_H_
