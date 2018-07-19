#ifndef _CREATE_TENSOR_MESSAGE_REPLY_H_
#define _CREATE_TENSOR_MESSAGE_REPLY_H_

#include <string>
#include <vector>

namespace cirrus {

class CreateTensorMessageReply {
  public:
    const char* getReturnData() const {
      throw "fix";
      return nullptr;
    }

    uint32_t getReturnSize() const {
      throw "fix";
      return 0;
    }

    bool getBool() const {
      throw "fix";
      return true;
    }
//    CreateTensorMessageReply(const std::string& tensor_name,
//                             const std::vector<uint32_t>& tensor_dim);
//    explicit CreateTensorMessageReply(const char* data);
//
//    std::string get_name() const;
//    std::vector<uint32_t> get_tensor_dims() const;
//
//    uint32_t get_data_size() const;
//    const char* get_data() const;

  private:
//    std::string name;
//    const std::vector<uint32_t>& tensor_dim;
};

} // namespace cirrus

#endif  // _CREATE_TENSOR_MESSAGE_REPLY_H_
