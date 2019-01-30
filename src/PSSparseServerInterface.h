#ifndef PS_SPARSE_SERVER_INTERFACE_H_
#define PS_SPARSE_SERVER_INTERFACE_H_

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "LDAModel.h"
#include "LDAStatistics.h"
#include "Model.h"
#include "ModelGradient.h"
#include "SparseLRModel.h"
#include "SparseMFModel.h"
#include "Utils.h"

namespace cirrus {

class PSSparseServerInterface {
 public:
  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  void connect();

  void send_lr_gradient(const LRSparseGradient&);
  void send_mf_gradient(const MFSparseGradient&);
  void send_lda_update(char* gradient_mem,
                       int total_sampled_tokens,
                       int total_sampled_docs,
                       uint32_t to_send_size);

  SparseLRModel get_lr_sparse_model(const SparseDataset& ds, const Configuration& config);
  void get_lr_sparse_model_inplace(const SparseDataset& ds, SparseLRModel&, const Configuration& config);
  SparseMFModel get_sparse_mf_model(const SparseDataset& ds, uint32_t, uint32_t);

  /**
   * Request a new vocabulary slice from the server
   *
   * @return to_receive_size: the size of compressed vocab slice
   * @return uncompressed_size: the size of original vocab slice
   */
  char* get_lda_model(uint32_t& to_receive_size,
                      uint32_t& uncompressed_size);
  /**
   * Request the pre-cached word indices for the current S3 object
   * from the server
   *
   * @param local_model_id the current vocab slice id
   */
  char* get_slices_indices(int local_model_id);

  /**
   * Send the log-likelihood update corresponding to
   * the doc-ll of the given S3_object
   *
   * @param local_model_id the current vocab slice id
   */
  void update_ll_ndt(int local_model_id, double ll);

  /**
   * Send the time distribution on the worker side
   * XXX can be removed (for the use of benchmarking only)
   *
   * @param sampling_time: the sampling time
   * @param comm_time: the communication time
   */
  void send_time_dist(double sampling_time, double comm_time);

  std::unique_ptr<CirrusModel> get_full_model(bool isCollaborativeFiltering); //XXX use a better argument here

  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);

  // time variable for the use of benchmarking
  double time_send = 0.0, time_receive = 0.0, num_get_lda_model = 0.0,
         time_whole = 0.0, time_create_model = 0.0, time_receive_size = 0.0;

  // the last-assigned slice id for the current worker
  int slice_id = -1;
  /*
   * Set key-value pair
   * @param key Key name
   * @param value Value is a blob of bytes
   * @param size Size of value in bytes
   */
  void set_value(const std::string& key, char* data, uint32_t size);

  /*
   * Get key-value pair
   * @param key Key name
   * @return Returns pointer to raw value
   */
  std::pair<std::shared_ptr<char>, uint32_t> get_value(const std::string& key);

  /*
   * Marks task as running on the parameter server
   * Used to guarantee there are no duplicate tasks
   * @param id Unique id of task
   * @param remaining_time_sec Worker timeout in secs
   * @return 0 if success, 1 otherwise
   */
  uint32_t register_task(uint32_t id, uint32_t remaining_time_sec);

  /*
   * Marks task as terminated
   * Used by the PS to update the number of active workers
   * @param id Unique id of task
   * @return 0 if success, 1 if already deregistered, 2 if not registered
   */
  uint32_t deregister_task(uint32_t id);

 private:
  std::string ip;
  int port;
  int sock = -1;
  struct sockaddr_in serv_addr;
};

} // namespace cirrus

#endif //  PS_SPARSE_SERVER_INTERFACE_H_
