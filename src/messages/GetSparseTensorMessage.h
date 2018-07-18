#ifndef _GET_SPARSE_TENSOR_MESSAGE_H_
#define _GET_SPARSE_TENSOR_MESSAGE_H_

#include <string>

namespace cirrus {

class GetSparseTensorMessage {
  public:
    GetSparseTensorMessage(const std::string& name) { throw "fix"; }

//    uint32_t get_name_size() const;
//    char* get_name_data() const;
  private:

};

}

#endif  // _GET_SPARSE_TENSOR_MESSAGE_H_
