#ifndef MULTIPLE_PS_SPARSE_SERVER_INTERFACE
#define MULTIPLE_PS_SPARSE_SERVER_INTERFACE

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include "ModelGradient.h"
#include "PSSparseServerInterface.h"
#include "SparseLRModel.h"
#include "Utils.h"

namespace cirrus {

class MultiplePSSparseServerInterface {
 public:
  MultiplePSSparseServerInterface(std::vector<std::string> ps_ips,
                                  std::vector<uint64_t> ps_ports);
  void send_gradient(const LRSparseGradient& gradient);
  std::unique_ptr<CirrusModel> get_full_model();
  SparseLRModel get_lr_sparse_model(const SparseDataset& ds,
                                    const Configuration& config);
  void get_lr_sparse_model(const SparseDataset& ds,
                           SparseLRModel& model,
                           const Configuration& config);

 private:
  std::vector<PSSparseServerInterface*> psints;
};

}  // namespace cirrus
#endif  //  MULTIPLE_PS_SPARSE_SERVER_INTERFACE
