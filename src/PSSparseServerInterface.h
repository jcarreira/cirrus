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
  PSSparseServerInterface() = default;
  PSSparseServerInterface(const std::string& ip, int port);
  virtual ~PSSparseServerInterface();

  virtual void connect();

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
                                           const Configuration& config,
                                           uint32_t user_base,
                                           uint32_t mb_size);

  virtual std::unique_ptr<CirrusModel> get_full_model(
      const Configuration& config,
      bool isCollaborativeFiltering);  // XXX use a better argument here

  void set_status(uint32_t id, uint32_t status);
  uint32_t get_status(uint32_t id);

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
  void get_lr_sparse_model_inplace_sharded(SparseLRModel& lr_model,
                                           const Configuration& config,
                                           char* msg_begin,
                                           uint32_t num_weights,
                                           int server_id,
                                           int num_ps);

  void get_mf_sparse_model_inplace_sharded(SparseMFModel& model,
                                           std::vector<uint32_t> seen_indices,
                                           const Configuration& config,
                                           char* msg_begin,
                                           uint32_t num_users,
                                           uint32_t num_items,
                                           int server_id,
                                           int num_ps);

  void get_full_model_inplace(std::unique_ptr<cirrus::SparseLRModel>& model,
                              int server_id,
                              int num_ps);
  void get_full_model_inplace(std::unique_ptr<cirrus::SparseMFModel>& model,
                              const Configuration& config,
                              int server_id,
                              int num_ps);

  int send_all_wrapper(void* data, uint32_t size);

  std::string ip;
  int port;
  int sock = -1;
  struct sockaddr_in serv_addr;
};

} // namespace cirrus

#endif //  PS_SPARSE_SERVER_INTERFACE_H_
