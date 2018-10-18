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
#include <iostream>
#include <memory>
#include "ModelGradient.h"
#include "Utils.h"
#include "SparseLRModel.h"
#include "SparseMFModel.h"
#include "Model.h"

namespace cirrus {

class PSSparseServerInterface {
  friend class MultiplePSSparseServerInterface;

 public:
  PSSparseServerInterface(){};
  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  void connect();

  virtual void send_lr_gradient(const LRSparseGradient&);
  virtual void send_mf_gradient(const MFSparseGradient&);

  virtual SparseLRModel get_lr_sparse_model(const SparseDataset& ds,
                                            const Configuration& config);
  virtual void get_lr_sparse_model_inplace(const SparseDataset& ds,
                                           SparseLRModel&,
                                           const Configuration& config);
  virtual SparseMFModel get_sparse_mf_model(const SparseDataset& ds,
                                            uint32_t,
                                            uint32_t);
  virtual void get_mf_sparse_model_inplace(const SparseDataset& ds,
                                           SparseMFModel&,
                                           const Configuration& config, uint32_t user_base, uint32_t mb_size);
  
  virtual std::unique_ptr<CirrusModel> get_full_model(
      bool isCollaborativeFiltering);  // XXX use a better argument here

  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);

 private:
  void get_lr_sparse_model_inplace_sharded(SparseLRModel& lr_model,
                                           const Configuration& config,
                                           char* msg_begin,
                                           uint32_t num_weights,
                                           int server_id,
                                           int num_ps);

  void get_mf_sparse_model_inplace_sharded(SparseMFModel& model,
                                           const Configuration& config,
                                           char* msg_begin,
                                           uint32_t num_users,
                                           uint32_t num_items,
                                           int server_id,
                                           int num_ps);

  void get_full_model_inplace(std::unique_ptr<cirrus::SparseLRModel>& model,
                              int server_id,
                              int num_ps);

  int send_wrapper(uint32_t num, std::size_t size);
  int send_all_wrapper(char* data, uint32_t size);

  std::string ip;
  int port;
  int sock = -1;
  struct sockaddr_in serv_addr;
};

} // namespace cirrus

#endif //  PS_SPARSE_SERVER_INTERFACE_H_
