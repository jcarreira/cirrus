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

class MultiplePSSparseServerInterface : public PSSparseServerInterface {
 public:
  MultiplePSSparseServerInterface(const Configuration& config,
                                  const std::vector<std::string>& ps_ips,
                                  const std::vector<uint64_t>& ps_ports);

  void connect();

  void send_lr_gradient(const LRSparseGradient&);
  void send_mf_gradient(const MFSparseGradient&);

  SparseLRModel get_lr_sparse_model(const SparseDataset& ds,
                                    const Configuration& config);
  void get_lr_sparse_model_inplace(const SparseDataset& ds,
                                   SparseLRModel& model,
                                   const Configuration& config);
  SparseMFModel get_mf_sparse_model(const SparseDataset& ds,
                                    std::vector<uint32_t> seen_indices,
                                    const Configuration& config,
                                    uint32_t,
                                    uint32_t);
  void get_mf_sparse_model_inplace(const SparseDataset& ds,
                                   SparseMFModel& model,
                                   const Configuration& config,
                                   uint32_t user_base,
                                   uint32_t minibatch_size);
  std::unique_ptr<CirrusModel> get_full_model(const Configuration& config,
                                              bool isCollaborativeFiltering);

 private:
  std::vector<PSSparseServerInterface*> psints;
  int minibatch_size = -1;
};

}  // namespace cirrus
#endif  //  MULTIPLE_PS_SPARSE_SERVER_INTERFACE
