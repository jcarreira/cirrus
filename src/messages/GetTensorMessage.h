#ifndef _GET_TENSOR_MESSAGE_H_
#define _GET_TENSOR_MESSAGE_H_

#include <string>

namespace cirrus {

class GetTensorMessage {
  public:
    GetTensorMessage(const std::string& name) { throw "fix"; }

    uint32_t get_name_size() const;
    char* get_name_data() const;
  private:

};

}

#endif  // _GET_TENSOR_MESSAGE_H_
