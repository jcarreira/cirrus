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
#include "LDAModel.h"
#include "LDAStatistics.h"

namespace cirrus {

class PSSparseServerInterface {
 public:
  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  void connect();

  void send_lr_gradient(const LRSparseGradient&);
  void send_mf_gradient(const MFSparseGradient&);
  void send_lda_update(LDAUpdates&, int total_sampled_tokens);

  SparseLRModel get_lr_sparse_model(const SparseDataset& ds, const Configuration& config);
  void get_lr_sparse_model_inplace(const SparseDataset& ds, SparseLRModel&, const Configuration& config);
  SparseMFModel get_sparse_mf_model(const SparseDataset& ds, uint32_t, uint32_t);
  // void get_lda_model(LDAStatistics&, int update_bucket, std::unique_ptr<LDAModel>& model);
  char* get_lda_model(int local_model_id, uint32_t& to_receive_size, uint32_t& uncompressed_size);

  char* get_slices_indices(int local_model_id);
  void update_ll_ndt(int bucket_id, double ll);

  std::unique_ptr<CirrusModel> get_full_model(bool isCollaborativeFiltering); //XXX use a better argument here

  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);

  double time_send = 0.0, time_receive = 0.0, num_get_lda_model = 0.0, time_whole = 0.0, time_create_model = 0.0, time_receive_size = 0.0;
  int slice_id = -1;

 private:
  std::string ip;
  int port;
  int sock = -1;
  struct sockaddr_in serv_addr;
};

} // namespace cirrus

#endif //  PS_SPARSE_SERVER_INTERFACE_H_
