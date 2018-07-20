#ifndef _GET_SPARSE_TENSOR_MESSAGE_H_
#define _GET_SPARSE_TENSOR_MESSAGE_H_

#include <string>
#include <vector>
#include <memory>

namespace cirrus {

class GetSparseTensorMessage {
  public:
    GetSparseTensorMessage(const std::string& name, const std::vector<uint32_t>& indexes);

    uint32_t getDataSize() const;
    char* getData() const;
  private:
    void buildData();

    const std::string& name;
    const std::vector<uint32_t>& indexes;

    std::shared_ptr<char[]> msg_data;
};

}

#endif  // _GET_SPARSE_TENSOR_MESSAGE_H_
