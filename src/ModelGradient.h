#ifndef _MODELGRADIENT_H_
#define _MODELGRADIENT_H_

#include <cstdint>
#include <cassert>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <config.h>
#include <memory>
#include <map>

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

  int update(const LDAUpdates& gradient, std::vector<int>& vocabs_to_update);
  char* get_partial_model(const char* slice, uint32_t& to_send_size);

  void get_nvt(std::vector<int>& nvt) { nvt = change_nvt; }
  void get_nt(std::vector<int>& nt) { nt = change_nt; }
  int get_vocab_map(int key) { return slice_map[key]; }

  void setVersion(int v) { version = v; }
  int getVersion() const { return version; }

  void print() const;

  std::map<int, int> slice_map;

  // void check_values() const;
 protected:
  std::vector<int> change_nvt, change_nt;  //< weights of the LDA update
  std::vector<int> slice;
  uint64_t version = 0;
  int update_bucket = 0;
};

} // namespace cirrus

#endif  // _MODELGRADIENT_H_
