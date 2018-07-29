#ifndef PS_SPARSE_SERVER_INTERFACE_H_
#define PS_SPARSE_SERVER_INTERFACE_H_

#include "Model.h"
#include "ModelGradient.h"
#include "SparseLRModel.h"
#include "SparseMFModel.h"
#include "Utils.h"
#include "common/schemas/PSMessage_generated.h"
#include "common/schemas/WorkerMessage_generated.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace cirrus {

class PSSparseServerInterface {
 public:
  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  void send_gradient(const ModelGradient& gradient,
                     message::WorkerMessage::ModelType mt);

  void connect();

  void send_lr_gradient(const LRSparseGradient&);
  void send_mf_gradient(const MFSparseGradient&);

  SparseLRModel get_lr_sparse_model(const SparseDataset& ds,
                                    const Configuration& config);
  void get_lr_sparse_model_inplace(const SparseDataset& ds,
                                   SparseLRModel&,
                                   const Configuration& config);
  SparseMFModel get_sparse_mf_model(const SparseDataset& ds,
                                    uint32_t,
                                    uint32_t);

  std::unique_ptr<CirrusModel> get_full_model(
      bool isCollaborativeFiltering);  // XXX use a better argument here

  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);

 private:
  std::string ip;
  int port;
  int sock = -1;
  struct sockaddr_in serv_addr;
};

}  // namespace cirrus

#endif  //  PS_SPARSE_SERVER_INTERFACE_H_
