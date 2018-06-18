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

namespace cirrus {

class PSSparseServerInterface {
 public:
  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  /**
    * Create a multidimensional tensor
    * @param tensor_name Name of the new tensor
    */
  bool create_tensor(const std::string& tensor_name, const std::vector<uint32_t>& tensor_dims);

  /**
    * Updates a parameter server tensor with
    * given update_tensor
    * @param tensor_name Name of the tensor to be update
    * @param update_tensor Name of the tensor gradient
    */
  bool update_tensor(
          const std::string& tensor_name,
          const SparseTensor& update_tensor);

  /** gets a full tensor
    * @param tensor_name name of the tensor 
    */
  Tensor get_tensor(const std::string& tensor_name);

  /** Gets a sparse tensor (1d tensor)
    * @param tensor_name name of the tensor 
    */
  SparseTensor get_sparse_tensor(
          const std::string& tensor_name,
          const std::vector<uint32_t>& indexes);
  
  /** Gets a sparse tensor (2d tensor)
    * @param tensor_name name of the tensor 
    */
  SparseTensor get_sparse_tensor(
          const std::string& tensor_name,
          const std::vector<std::tuple<uint32_t, uint32_t>>& indexes);
  
  /** Gets a sparse tensor (3d tensor)
    * @param tensor_name name of the tensor 
    */
  SparseTensor get_sparse_tensor(
          const std::string& tensor_name,
          const std::vector<
               std::tuple<uint32_t, uint32_t, uint32_t>>& indexes);

  void send_lr_gradient(const LRSparseGradient&);
  void send_mf_gradient(const MFSparseGradient&);
  
  SparseLRModel get_lr_sparse_model(const SparseDataset& ds, const Configuration& config);
  void get_lr_sparse_model_inplace(const SparseDataset& ds, SparseLRModel&, const Configuration& config);
  SparseMFModel get_sparse_mf_model(const SparseDataset& ds, uint32_t, uint32_t);

  std::unique_ptr<CirrusModel> get_full_model(bool isCollaborativeFiltering); //XXX use a better argument here

  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);

 private:
  std::string ip;
  int port;
  int sock = -1;
};

} // namespace cirrus

#endif //  PS_SPARSE_SERVER_INTERFACE_H_
