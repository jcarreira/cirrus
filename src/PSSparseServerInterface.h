#ifndef PS_SPARSE_SERVER_INTERFACE_H_
#define PS_SPARSE_SERVER_INTERFACE_H_

#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include <string>
#include <iostream>
#include <memory>
#include <ModelGradient.h>
#include <Utils.h>
#include <SparseLRModel.h>
#include <SparseMFModel.h>
#include <Model.h>
#include <Tensor.h>
#include <SparseTensor.h>

namespace cirrus {

class PSSparseServerInterface {
 public:
  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  /**
    * Create a multidimensional tensor
    * @param tensor_name Name of the new tensor
    */
  bool createTensor1D(const std::string& tensor_name,
                      uint32_t tensor_dim);
  bool createTensor2D(const std::string& tensor_name,
                      std::pair<uint32_t, uint32_t> tensor_dims);

  /**
    * Updates a parameter server tensor with
    * given add_tensor
    * @param tensor_name Name of the tensor to be update
    * @param add_tensor
    */
  bool addTensor1D(
          const std::string& tensor_name,
          const SparseTensor1D& add_tensor);
//  bool add_tensor2D(
//          const std::string& tensor_name,
//          const SparseTensor2D& add_tensor);

  /** gets a full tensor
    * @param tensor_name name of the tensor 
    */
  Tensor1D getTensor1D(const std::string& tensor_name);

  /** Gets a sparse tensor (1d tensor)
    * @param tensor_name name of the tensor 
    */
  SparseTensor1D getSparseTensor1D(
          const std::string& tensor_name,
          const std::vector<uint32_t>& indexes);
  
  /** Gets a sparse tensor (2d tensor)
    * @param tensor_name name of the tensor 
    */
  SparseTensor2D getSparseTensor2D(
          const std::string& tensor_name,
          const std::vector<std::pair<uint32_t, uint32_t>>& indexes);

  void setStatus(uint32_t id, uint32_t status);
  uint32_t getStatus(uint32_t id);

 private:
  std::string ip;
  int port;
  int sock = -1;
};

} // namespace cirrus

#endif //  PS_SPARSE_SERVER_INTERFACE_H_
