#ifndef _MODELGRADIENT_H_
#define _MODELGRADIENT_H_

#include <cstdint>
#include <cassert>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <array>
#include <config.h>
#include <memory>
#include <map>
// #include <codecfactory.h>

namespace cirrus {

/**
  * This class is a base class for a model's gradient
  */
class ModelGradient {
  public:
    ModelGradient() : version(0) {}
    virtual ~ModelGradient() = default;

    /**
     * Serialize gradient
     * @param mem Pointer where to serialize the gradient
     */
    virtual void serialize(void* mem) const = 0;

    /**
     * Get the size of the gradient when serialized
     * @returns Size of serialized gradient
     */
    virtual uint64_t getSerializedSize() const = 0;

    /**
     * Load gradient from serialized memory
     * @param mem Pointer to memory where the gradient lives serialized
     */
    virtual void loadSerialized(const void* mem) = 0;

    /**
     * Print gradient
     */
    virtual void print() const = 0;

    /**
     * Set gradient version
     */
    virtual void setVersion(uint64_t v) {
      version = v;
    }

    /**
     * Get version of gradient
     */
    virtual uint64_t getVersion() const {
      return version;
    }

    /**
     * Sanity check gradient values
     */
    virtual void check_values() const = 0;

  protected:
    uint64_t version = 0;  //< this gradient's version
};

class LRGradient : public ModelGradient {
 public:
    friend class LRModel;
    friend class SparseLRModel;

    virtual ~LRGradient() = default;

    LRGradient(LRGradient&& data);
    explicit LRGradient(const std::vector<FEATURE_TYPE>& data);
    explicit LRGradient(int d);

    LRGradient& operator=(LRGradient&& other);

    void loadSerialized(const void*);
    void serialize(void*) const override;
    uint64_t getSerializedSize() const override;

    void print() const override;
    void check_values() const override;
 protected:
    std::vector<FEATURE_TYPE> weights;  //< weights of the LR gradient
};

class LRSparseGradient : public ModelGradient {
 public:
    friend class AdaGrad;
    friend class Momentum;
    friend class SGD;
    friend class Nesterov;
    friend class LRModel;
    friend class SparseLRModel;

    virtual ~LRSparseGradient() = default;

    LRSparseGradient(LRSparseGradient&& data);
    explicit LRSparseGradient(const std::vector<std::pair<int, FEATURE_TYPE>>&& data);
    explicit LRSparseGradient(int d);

    LRSparseGradient& operator=(LRSparseGradient&& other);

    void loadSerialized(const void*);
    void serialize(void*) const override;
    uint64_t getSerializedSize() const override;

    void print() const override;
    void check_values() const override;
 protected:
    std::vector<std::pair<int, FEATURE_TYPE>> weights;  //< weights
};

class SoftmaxGradient : public ModelGradient {
 public:
    friend class SoftmaxModel;

    virtual ~SoftmaxGradient() = default;

    SoftmaxGradient(uint64_t nclasses, uint64_t d);
    explicit SoftmaxGradient(const std::vector<std::vector<FEATURE_TYPE>>&);

    void loadSerialized(const void*);
    void serialize(void*) const override;
    uint64_t getSerializedSize() const override;

    void print() const override;
    void check_values() const override;
 protected:
    // [D * K]
    std::vector<std::vector<FEATURE_TYPE>> weights;  //< weights for softmax gradient
};

class MFGradient : public ModelGradient {
 public:
    friend class MFModel;

    virtual ~MFGradient() = default;

    MFGradient(uint64_t nsamples, uint64_t nfactors);
    explicit MFGradient(const std::vector<std::vector<FEATURE_TYPE>>&);

    void loadSerialized(const void*);
    void serialize(void*) const override;
    uint64_t getSerializedSize() const override;

    void print() const override;
    void check_values() const override;
 protected:
    // [D * K]
    std::vector<std::vector<FEATURE_TYPE>> weights;
};

class MFSparseGradient : public ModelGradient {
 public:
    friend class MFModel;

    MFSparseGradient();
    virtual ~MFSparseGradient() = default;

    void loadSerialized(const void*);
    void serialize(void*) const override;
    uint64_t getSerializedSize() const override;

    void print() const {
      std::cout << users_bias_grad.size() << " / " << users_weights_grad.size() << std::endl;
      std::cout << items_bias_grad.size() << " / " << items_weights_grad.size() << std::endl;
    }
    void check_values() const override;
 public:
    void check() {
      assert(users_bias_grad.size() == users_weights_grad.size());
      assert(items_bias_grad.size() == items_weights_grad.size());
    }

    // [D * K]
    std::unordered_map<int, FEATURE_TYPE> users_bias_grad;
    std::unordered_map<int, FEATURE_TYPE> items_bias_grad;
    //std::vector<FEATURE_TYPE> items_bias_grad;

    // user id and then weights
    std::vector<
      std::pair<int, std::vector<FEATURE_TYPE>>> users_weights_grad;
    // item id and then weights
    std::vector<
      std::pair<int, std::vector<FEATURE_TYPE>>> items_weights_grad;
};

class LDAUpdates {
 public:
  friend class LDAModel;

  virtual ~LDAUpdates() = default;

  LDAUpdates() {}
  LDAUpdates(LDAUpdates&& data);
  LDAUpdates(const std::vector<int>& nvt,
             const std::vector<int>& nt,
             const std::vector<int>& s);
  LDAUpdates(const std::vector<int>& nvt,
             const std::vector<int>& nt,
             const std::vector<int>& s,
             int to_update);
  LDAUpdates(const std::vector<int>& nvt, const std::vector<int>& nt);
  LDAUpdates(int nvt_dim, int nt_dim, int slice_size);
  LDAUpdates(int to_update);

  LDAUpdates& operator=(LDAUpdates&& other);

  void loadSerialized(const char* mem);
  std::shared_ptr<char> serialize(uint32_t*);
  uint64_t getSerializedSize() const;

  void loadSparseSerialized(const char* mem);
  std::shared_ptr<char> serialize_sparse(uint32_t*);

  int update(const LDAUpdates& gradient, std::vector<int>& vocabs_to_update);
  char* get_partial_model(int slice_id, uint32_t& to_send_size, uint32_t& uncompressed_size, bool check_sparse);

  // void get_nvt(std::vector<int>& nvt) { nvt = change_nvt; }
  // void get_nt(std::vector<int>& nt) { nt = change_nt; }
  void get_slice(std::vector<int>& s) { s = slice; }
  void get_nvt_pointer(std::shared_ptr<std::vector<int>>& nvt_ptr)
                      {nvt_ptr = change_nvt_ptr;}
  void get_nt_pointer(std::shared_ptr<std::vector<int>>& nt_ptr)
                      {nt_ptr = change_nt_ptr;}
  int get_nvt_size() { return change_nvt_ptr->size(); }
  int get_nt_size() { return change_nt_ptr->size(); }
  int get_slice_size() { return slice.size(); }

  // void get_partial_nvt(std::vector<int>& nvt, std::vector<int>& local_slice);
  // void get_partial_sparse_nvt(
  //                 std::vector<std::vector<int>>& nvt_sparse,
  //                 std::vector<int>& local_slice);
  // void get_slice_map(std::array<int, int>& s) { s = slice_map; }
  // int get_vocab_map(int key) { return slice_map[key]; }

  int pre_assign_slices(int slice_size);

  void get_partial_sparse_nvt_ptr(
                    std::shared_ptr<std::vector<std::vector<int>>>& nvt_ptr,
                    const std::vector<int>& local_slice);

  void setVersion(int v) { version = v; }
  int getVersion() const { return version; }

  void print() const;

  double get_compress_rate() { return total_to_compress_size / total_time_to_compress; }
  double get_compress_effect() { return total_to_compress_size / total_compressed_size; }

  std::array<int, 1000000> slice_map, sparse_records;
  double time_whole = 0.0, time_find_partial = 0.0, time_compress = 0.0, counts = 0.0, time_temp=0.0, time_ttemp=0.0, time_nvt_find = 0.0, time_check_sparse = 0.0, time_serial_sparse = 0.0, time_check = 0.0;

  // void check_values() const;
 // protected:
 /**
   *
   * @variable change_nvt: the statistics of word counts over vocabularies and topics
   *           size: V * K where V is the size of vocabulary space
   * @variable change_nt: the statistics of word counts over topics
   *           size: K
   * @variable slice_: the local vocabulary space
   */
  std::shared_ptr<std::vector<int>> change_nvt_ptr, change_nt_ptr;
  std::shared_ptr<std::vector<std::vector<std::pair<int, int>>>> sparse_change_nvt_ptr;
  std::vector<std::vector<int>> change_nvt_indices;
  std::vector<std::vector<std::pair<int, int>>> sparse_nvt;
  // std::vector<int> change_nvt, change_nt;  //< weights of the LDA update
  std::vector<std::vector<int>> fixed_slices;
  std::array<int, 1000000> temp_look_up, sparse_look_up;
  std::vector<int> slice;
  uint64_t version = 0;
  int update_bucket = 0, temp_counter = 0;


  double total_to_compress_size = 0., total_compressed_size = 0., total_time_to_compress = 0.;
};

} // namespace cirrus

#endif  // _MODELGRADIENT_H_
