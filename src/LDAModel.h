#ifndef _LDAMODEL_H_
#define _LDAMODEL_H_

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <Model.h>
#include <LDADataset.h>
#include <ModelGradient.h>
// #include <LDAStatistics.h>

namespace cirrus {

/**
  * Latent Dirichlet Allocation model
  */
class LDAModel {
 public:
  LDAModel() {}
  /**
    * LDA Model constructor from serialized memory
    * @param info: Serialized memory containing local statistics
    *               (ndt, topics assignments, vocab slice)
    */
  LDAModel(const char* info);
  /**
    * LDA Model constructor from serialized memory
    * @param buffer: serialized memory containing global statistics
    *               (nvt, nt)
    * @param compressed_size: size of compressed mem
    * @param uncompressed_size: size of uncompressed mem
    * @param_id slice_id
    */
  void update_model(const char* buffer, int compressed_size, int uncompressed_size, int slice_id);
  /**
    * LDA sampling function (currently using SparseLDA Sampler)
    * Sampling is based on all the statistics stored in the object
    *
    * @param total_sampled_tokens: number of tokens sampled in this run
    * @param slice_indices: a list of cached indices for the current slice_id
    * @param to_send_size: the size of serialized update
    *
    * @return: serialized update to be sent to the server; corresponding
    *          changes have been applied to the statistics current model holds
    */
  char* sample_model(int& total_sampled_tokens, std::vector<int>& slice_indices, uint32_t& to_send_size);
  /**
   * Compute the loglikelihood for current document (local statistics)
   */
  double compute_ll_ndt();

  char* serialize_to_S3(uint64_t& to_send_size);

  void get_ndt(std::vector<std::vector<int>>& ndt_) { ndt_ = ndt; }
  void get_nt(std::vector<int>& nt_) { nt_ = nt; }
  void get_t(std::vector<int>& t_) { t_ = t; }

  int get_ndt_size() { return ndt.size(); }

  LDAModel& operator=(LDAModel& model);

protected:
  char* sample_thread(std::vector<int>& t,
                      std::vector<int>& d,
                      std::vector<int>& w,
                      std::vector<int>& nt,
                      std::vector<std::vector<int>>& nvt,
                      std::vector<std::vector<int>>& ndt,
                      std::vector<int>& slice,
                      int& total_sampled_tokens,
                      std::vector<int>& slice_indices,
                      uint32_t& to_send_size);
  /**
    * K_: number of topics
    * V_: global word space dimension
    * local_V: local word space dimension
    */
  int K_, V_, update_bucket, local_V;
  /**
    * Set to fixed constants:
    * alpha = .1, eta = .01
    */
  double alpha = .1, eta = .01;
  /**
    * topic assignments
    */
  std::vector<int> t, d, w;
  /**
    * statistics needed for sampling
    */
  std::vector<std::vector<int>> ndt, nvt;
  std::vector<int> nt;
  /**
    * indices of nonzero entries for each statistics
    */
  std::vector<std::vector<int>> nz_ndt_indices, nz_nvt_indices;
  std::vector<int> nz_nt_indices;
  /**
    * helpers tracking the order of nonzero indices to
    * speed up the later delete operations
    */
  std::vector<std::array<int, 1000>> nz_ndt_indices_check, nz_nvt_indices_check;
  std::array<int, 1000> nz_nt_indices_check;
  std::vector<int> nz_ndt_indices_counter, nz_nvt_indices_counter;
  /**
    * vocabulary slice containing word indices
    * that are being considered currently
    */
  std::vector<int> slice;

};
}
#endif
