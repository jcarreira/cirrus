#ifndef _GET_SPARSE_TENSOR_MESSAGE_H_
#define _GET_SPARSE_TENSOR_MESSAGE_H_

#include <string>
#include <vector>
#include <memory>

namespace cirrus {

class GetSparseTensorMessage {
  public:
    GetSparseTensorMessage(const std::string& name, const std::vector<uint32_t>& indexes);

    uint32_t get_data_size() const;
    char* get_data() const;
  private:
    void build_data();

    const std::string& name;
    const std::vector<uint32_t>& indexes;

    std::shared_ptr<char[]> msg_data;
};

}

#endif  // _GET_SPARSE_TENSOR_MESSAGE_H_
