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
 public:

  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  // we only support LR with EFS for now
#ifdef USE_EFS
  void send_lr_gradient(const LRSparseGradient&);
  SparseLRModel get_lr_sparse_model(const SparseDataset& ds, const Configuration& config) {
      throw "not supported";
      return SparseLRModel(0);
  }
  void get_lr_sparse_model_inplace(const SparseDataset& ds, SparseLRModel&, const Configuration& config);
  std::unique_ptr<CirrusModel> get_full_model(bool isCollaborativeFiltering);
  
  void send_mf_gradient(const MFSparseGradient&) {}
  SparseMFModel get_sparse_mf_model(const SparseDataset& ds, uint32_t, uint32_t) { return SparseMFModel(1,1,1); }
  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);
#else
  void send_lr_gradient(const LRSparseGradient&);
  void send_mf_gradient(const MFSparseGradient&);
  
  SparseLRModel get_lr_sparse_model(const SparseDataset& ds, const Configuration& config);
  void get_lr_sparse_model_inplace(const SparseDataset& ds, SparseLRModel&, const Configuration& config);
  SparseMFModel get_sparse_mf_model(const SparseDataset& ds, uint32_t, uint32_t);

  std::unique_ptr<CirrusModel> get_full_model(bool isCollaborativeFiltering); //XXX use a better argument here

  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);
#endif

 private:
  std::string ip;
  int port;
  int sock = -1;
};

} // namespace cirrus

#endif //  PS_SPARSE_SERVER_INTERFACE_H_
