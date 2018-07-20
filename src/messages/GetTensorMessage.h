#ifndef _GET_TENSOR_MESSAGE_H_
#define _GET_TENSOR_MESSAGE_H_

#include <string>

namespace cirrus {

class GetTensorMessage {
  public:
    GetTensorMessage(const std::string& name) { throw "fix"; }

    uint32_t getNameSize() const;
    char* getNameData() const;
  private:

};

}

#endif  // _GET_TENSOR_MESSAGE_H_
