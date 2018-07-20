#ifndef _CREATE_TENSOR_MESSAGE_H_
#define _CREATE_TENSOR_MESSAGE_H_

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace cirrus {

class CreateTensorMessage {
  public:
    CreateTensorMessage(const std::string& tensor_name, uint32_t tensor_dim);
    CreateTensorMessage(const std::string& tensor_name, const std::vector<uint32_t>& tensor_dim);
    explicit CreateTensorMessage(const char* data);

    std::string getName() const;
    std::vector<uint32_t> getTensorDims() const;

    uint32_t getTotalDataSize() const;
    
    const char* getData() const;

  private:
    void populateData();
    uint32_t getDataSize() const;

    std::string tensor_name;
    std::vector<uint32_t> tensor_dim;

    std::shared_ptr<char[]> data;
};

} // namespace cirrus

#endif  // _CREATE_TENSOR_MESSAGE_H_
