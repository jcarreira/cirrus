#include <ModelGradient.h>
#include <iostream>
#include <algorithm>
#include <Utils.h>
#include <cassert>
#include "Constants.h"

namespace cirrus {

/**
 * LRGradient
 */

LRGradient::LRGradient(LRGradient&& other) {
  weights = std::move(other.weights);
  version = other.version;
}

LRGradient::LRGradient(const std::vector<FEATURE_TYPE>& data) :
  weights(data) {
  }

LRGradient::LRGradient(int d) {
  weights.resize(d);
  version = 0;
}

LRGradient& LRGradient::operator=(LRGradient&& other) {
  weights = std::move(other.weights);
  version = other.version;
  return *this;
}

void LRGradient::loadSerialized(const void* mem) {
  version = *reinterpret_cast<const uint32_t*>(mem);
  mem = reinterpret_cast<const void*>(
      (reinterpret_cast<const char*>(mem) + sizeof(uint32_t)));
  const FEATURE_TYPE* data = reinterpret_cast<const FEATURE_TYPE*>(mem);
  std::copy(data, data + weights.size(), weights.begin());
}

/** Format:
 * version (uint32_t)
 * vector of weights (FEATURE_TYPE * n)
 */
void LRGradient::serialize(void* mem) const {
  *reinterpret_cast<uint32_t*>(mem) = version;
  mem = reinterpret_cast<void*>(
      (reinterpret_cast<char*>(mem) + sizeof(uint32_t)));
  FEATURE_TYPE* data = reinterpret_cast<FEATURE_TYPE*>(mem);

#if 0
  for (const auto& w : weights) {
    if (w == 0) {
      throw std::runtime_error("0 weight");
    }
  }
#endif

  std::copy(weights.begin(), weights.end(), data);
}

uint64_t LRGradient::getSerializedSize() const {
  return weights.size() * sizeof(FEATURE_TYPE) + sizeof(uint32_t);
}

void LRGradient::print() const {
  std::cout << "Printing LRGradient. version: " << version << std::endl;
  for (const auto &v : weights) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
}

void LRGradient::check_values() const {
  for (const auto& w : weights) {
    if (std::isnan(w) || std::isinf(w)) {
      throw std::runtime_error("LRGradient::check_values error");
    }
  }
}



/**
 * LRSparseGradient
 */

LRSparseGradient::LRSparseGradient(LRSparseGradient&& other) {
  weights = std::move(other.weights);
  version = other.version;
}

LRSparseGradient::LRSparseGradient(
    const std::vector<std::pair<int, FEATURE_TYPE>>&& data) :
  weights(data) {
  }

LRSparseGradient::LRSparseGradient(int d) {
  weights.resize(d);
  version = 0;
}

LRSparseGradient& LRSparseGradient::operator=(LRSparseGradient&& other) {
  weights = std::move(other.weights);
  version = other.version;
  return *this;
}

/**
 *
 */
void LRSparseGradient::loadSerialized(const void* mem) {
  // load version and number of weights
  version = load_value<int>(mem);
  int num_weights = load_value<int>(mem);
  assert(num_weights > 0 && num_weights < 10000000);

  int size = num_weights * (sizeof(FEATURE_TYPE)+sizeof(int)) + 2 * sizeof(int);
  char* data_begin = (char*)mem;

  //std::cout << "Number of weights: " << num_weights << std::endl;
  //std::cout << "Version: " << version << std::endl;
  //std::cout << "size: " << size << std::endl;

  // clear weights
  weights.resize(0);

  for (int i = 0; i < num_weights; ++i) {
    assert(std::distance(data_begin, (char*)mem) < size);
    int index = load_value<int>(mem);
    FEATURE_TYPE w = load_value<FEATURE_TYPE>(mem);
    weights.push_back(std::make_pair(index, w));
  }
}

/** Format:
 * version (int)
 * number of weights (int)
 * list of weights: index1 (int) weight1 (FEATURE_TYPE) | index2 (int) weight2 (FEATURE_TYPE) | ..
 */
void LRSparseGradient::serialize(void* mem) const {
  store_value<int>(mem, version);
  store_value<int>(mem, weights.size());

  for (const auto& w : weights) {
    int index = w.first;
    FEATURE_TYPE v = w.second;
    store_value<int>(mem, index);
    store_value<FEATURE_TYPE>(mem, v);
  }
}

uint64_t LRSparseGradient::getSerializedSize() const {
  return weights.size() * (sizeof(FEATURE_TYPE) + sizeof(int)) + // pairs (index, weight value)
    sizeof(int) * 2; // version + number of weights
}

void LRSparseGradient::print() const {
  std::cout << "Printing LRSparseGradient. version: " << version << std::endl;
  for (const auto &v : weights) {
    std::cout << "(" << v.first << "," << v.second << ") ";
  }
  std::cout << std::endl;
}

void LRSparseGradient::check_values() const {
  for (const auto& w : weights) {
    if (std::isnan(w.second) || std::isinf(w.second)) {
      throw std::runtime_error("LRSparseGradient::check_values error");
    }
  }
}

/**
 * SOFTMAX
 *
 */

SoftmaxGradient::SoftmaxGradient(uint64_t nclasses, uint64_t d) {
  weights.resize(d);
  for (auto& v : weights) {
    v.resize(nclasses);
  }
}

SoftmaxGradient::SoftmaxGradient(const std::vector<std::vector<FEATURE_TYPE>>& w) {
  weights = w;
}

void SoftmaxGradient::serialize(void* mem) const {
  *reinterpret_cast<uint32_t*>(mem) = version;
  mem = reinterpret_cast<void*>(
      (reinterpret_cast<char*>(mem) + sizeof(uint32_t)));
  FEATURE_TYPE* data = reinterpret_cast<FEATURE_TYPE*>(mem);

  for (const auto& v : weights) {
    std::copy(v.begin(), v.end(), data);
    data += v.size();
  }
}

uint64_t SoftmaxGradient::getSerializedSize() const {
  return weights.size() * weights[0].size() * sizeof(FEATURE_TYPE)
    + sizeof(uint32_t);
}

void SoftmaxGradient::loadSerialized(const void* mem) {
  version = *reinterpret_cast<const uint32_t*>(mem);
  mem = reinterpret_cast<const void*>(
      (reinterpret_cast<const char*>(mem) + sizeof(uint32_t)));
  const FEATURE_TYPE* data = reinterpret_cast<const FEATURE_TYPE*>(mem);

  for (auto& v : weights) {
    std::copy(data, data + v.size(), v.begin());
    data += v.size();
  }
}

void SoftmaxGradient::print() const {
  std::cout
    << "SoftmaxGradient (" << weights.size() << "x"
    << weights[0].size() << "): " << std::endl;
  for (const auto &v : weights) {
    for (const auto &vv : v) {
      std::cout << vv << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void SoftmaxGradient::check_values() const {
  for (const auto &v : weights) {
    for (const auto &vv : v) {
      if (std::isnan(vv) || std::isinf(vv)) {
        throw std::runtime_error("SoftmaxGradient::check_values error");
      }
    }
  }
}

/**
 * MFGradient
 *
 */

MFGradient::MFGradient(uint64_t nclasses, uint64_t d) {
  weights.resize(d);
  for (auto& v : weights) {
    v.resize(nclasses);
  }
}

MFGradient::MFGradient(const std::vector<std::vector<FEATURE_TYPE>>& w) {
  weights = w;
}

void MFGradient::serialize(void* mem) const {
  *reinterpret_cast<uint32_t*>(mem) = version;
  mem = reinterpret_cast<void*>(
      (reinterpret_cast<char*>(mem) + sizeof(uint32_t)));
  FEATURE_TYPE* data = reinterpret_cast<FEATURE_TYPE*>(mem);

  for (const auto& v : weights) {
    std::copy(v.begin(), v.end(), data);
    data += v.size();
  }
}

uint64_t MFGradient::getSerializedSize() const {
  return weights.size() * weights[0].size() * sizeof(FEATURE_TYPE)
    + sizeof(uint32_t);
}

void MFGradient::loadSerialized(const void* mem) {
  version = *reinterpret_cast<const uint32_t*>(mem);
  mem = reinterpret_cast<const void*>(
      (reinterpret_cast<const char*>(mem) + sizeof(uint32_t)));
  const FEATURE_TYPE* data = reinterpret_cast<const FEATURE_TYPE*>(mem);

  for (auto& v : weights) {
    std::copy(data, data + v.size(), v.begin());
    data += v.size();
  }
}

void MFGradient::print() const {
  std::cout
    << "MFGradient (" << weights.size() << "x"
    << weights[0].size() << "): " << std::endl;
  for (const auto &v : weights) {
    for (const auto &vv : v) {
      std::cout << vv << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void MFGradient::check_values() const {
  for (const auto &v : weights) {
    for (const auto &vv : v) {
      if (std::isnan(vv) || std::isinf(vv)) {
        throw std::runtime_error("MFGradient::check_values error");
      }
    }
  }
}

MFSparseGradient::MFSparseGradient() {
  //users_bias_grad.resize(nusers);
  //users_weights_grad.resize(nusers);

  //items_bias_grad.resize(nitems);
  //items_weights_grad.resize(nitems);
}

/** FORMAT of the Matrix Factorization sparse gradient
 * number of users (uint32_t)
 * number of items (uint32_t)
 * user_bias [# users] (FEATURE_TYPE)
 * item_bias [# items] (FEATURE_TYPE)
 * user weights grad id (uint32_t) and [# users * NUM_FACTORS] (uint32_t + FEATURE_TYPE)
 * item_weights_grad id (uint32_t) [# items * NUM_FACTORS] (uint32_t + FEATURE_TYPE)
 */
uint64_t MFSparseGradient::getSerializedSize() const {
  return sizeof(uint32_t) * (2 + 2) // also count magic values
    + users_bias_grad.size() * (sizeof(int) + sizeof(FEATURE_TYPE))
    + items_bias_grad.size() * (sizeof(int) + sizeof(FEATURE_TYPE))
    + users_weights_grad.size() * (sizeof(int) + NUM_FACTORS * sizeof(FEATURE_TYPE))
    + items_weights_grad.size() *  (sizeof(int) + NUM_FACTORS * sizeof(FEATURE_TYPE));
}

void MFSparseGradient::serialize(void *mem) const {
  store_value<uint32_t>(mem, MAGIC_NUMBER); // magic value
  store_value<uint32_t>(mem, users_bias_grad.size());
  store_value<uint32_t>(mem, items_bias_grad.size());

  // First we serialize the bias values
  for (const auto& user_bias : users_bias_grad) {
    store_value<int>(mem, user_bias.first);
    store_value<FEATURE_TYPE>(mem, user_bias.second);
  }
  for (const auto& bias_grad : items_bias_grad) {
    store_value<int>(mem, bias_grad.first);
    store_value<FEATURE_TYPE>(mem, bias_grad.second);
  }

  // Second we serialize the weight gradients
  assert(users_weights_grad.size() == users_bias_grad.size());
  for (const auto& user : users_weights_grad) {
    store_value<int>(mem, user.first);
    assert(user.second.size() == NUM_FACTORS);
    for (const auto& weight_grad : user.second) {
      store_value<FEATURE_TYPE>(mem, weight_grad);
    }
  }

  assert(items_weights_grad.size() == items_bias_grad.size());
  for (const auto& item : items_weights_grad) {
    //std::cout << "Serializing itemId: " << item.first << std::endl;
    store_value<int>(mem, item.first);
    assert(item.second.size() == NUM_FACTORS);
    for (const auto& weight_grad : item.second) {
      store_value<FEATURE_TYPE>(mem, weight_grad);
    }
  }
  store_value<uint32_t>(mem, 0x1338); // magic value
}

void MFSparseGradient::loadSerialized(const void* mem) {
  uint32_t magic_value = load_value<uint32_t>(mem);
  assert(magic_value == MAGIC_NUMBER);
  uint32_t users_size = load_value<uint32_t>(mem);
  uint32_t items_size = load_value<uint32_t>(mem);
  //users_bias_grad.reserve(users_size);
  //items_bias_grad.reserve(items_size);

  for (uint32_t i = 0; i < users_size; ++i) {
    int user_id = load_value<int>(mem);
    FEATURE_TYPE user_bias = load_value<FEATURE_TYPE>(mem);
    users_bias_grad[user_id] = user_bias;
  }
  for (uint32_t i = 0; i < items_size; ++i) {
    int item_id = load_value<int>(mem);
    FEATURE_TYPE item_grad = load_value<FEATURE_TYPE>(mem);
    items_bias_grad[item_id] = item_grad;
  }
  for (uint32_t i = 0; i < users_size; ++i) {
    std::pair<int, std::vector<FEATURE_TYPE>> user_weights_grad;
    user_weights_grad.first = load_value<int>(mem);
    user_weights_grad.second.reserve(NUM_FACTORS);
    for (uint32_t j = 0; j < NUM_FACTORS; ++j) {
      FEATURE_TYPE weight = load_value<FEATURE_TYPE>(mem);
      user_weights_grad.second.push_back(weight);
    }
    users_weights_grad.push_back(user_weights_grad);
  }

  for (uint32_t i = 0; i < items_size; ++i) {
    std::pair<int, std::vector<FEATURE_TYPE>> item_weights_grad;
    item_weights_grad.first = load_value<int>(mem);
    //std::cout << "loadSerialized itemId: " << item_weights_grad.first << std::endl;
    item_weights_grad.second.reserve(NUM_FACTORS);
    for (uint32_t j = 0; j < NUM_FACTORS; ++j) {
      FEATURE_TYPE weight = load_value<FEATURE_TYPE>(mem);
      item_weights_grad.second.push_back(weight);
    }
    items_weights_grad.push_back(item_weights_grad);
  }
  magic_value = load_value<uint32_t>(mem);
  assert(magic_value == 0x1338);
}

void MFSparseGradient::check_values() const {
  for (const auto& user : users_weights_grad) {
    if (user.first < 0) {
      throw std::runtime_error("Wrong id");
    }
  }
  for (const auto& item : items_weights_grad) {
    if (item.first < 0) {
      throw std::runtime_error("Wrong id");
    }
  }
}

LDAUpdates::LDAUpdates(LDAUpdates&& other) {
  change_nvt = std::move(other.change_nvt);
  change_nt = std::move(other.change_nt);
  slice = std::move(other.slice);
  version = other.version;
}

LDAUpdates::LDAUpdates(const std::vector<int>& nvt, const std::vector<int>& nt)
    : change_nvt(nvt), change_nt(nt) {
  slice.clear();
}

LDAUpdates::LDAUpdates(const std::vector<int>& nvt,
                       const std::vector<int>& nt,
                       const std::vector<int>& s)
    : change_nvt(nvt), change_nt(nt), slice(s) {}


LDAUpdates::LDAUpdates(const std::vector<int>& nvt,
                       const std::vector<int>& nt,
                       const std::vector<int>& s,
                       int to_update)
    : change_nvt(nvt), change_nt(nt), slice(s), update_bucket(to_update) {}

LDAUpdates::LDAUpdates(int to_update) : update_bucket(to_update) {
  slice.clear();
  change_nvt.clear();
  change_nt.clear();
}

LDAUpdates::LDAUpdates(int nvt_dim, int nt_dim, int slice_size) {
  change_nvt.resize(nvt_dim);
  change_nt.resize(nt_dim);
  slice.resize(slice_size);
  version = 0;
}

LDAUpdates& LDAUpdates::operator=(LDAUpdates&& other) {
  change_nvt = std::move(other.change_nvt);
  change_nt = std::move(other.change_nt);
  slice = std::move(other.slice);
  version = other.version;
  return *this;
}

void LDAUpdates::loadSerialized(const char* mem) {

  version = load_value<uint64_t>(mem);

  int len = load_value<uint32_t>(mem);
  change_nvt.clear();
  change_nvt.reserve(len);
  for (int i = 0; i < len; ++i) {
    int temp = load_value<int>(mem);
    change_nvt.push_back(temp);
  }

  len = load_value<uint32_t>(mem);
  change_nt.clear();
  change_nt.reserve(len);
  for (int i = 0; i < len; ++i) {
    int temp = load_value<int>(mem);
    change_nt.push_back(temp);
  }

  len = load_value<uint32_t>(mem);
  slice.clear();
  slice.reserve(len);
  for (int i = 0; i < len; ++i) {
    int temp = load_value<int>(mem);
    slice.push_back(temp);
  }

  update_bucket = load_value<uint32_t>(mem);

  int idx = 0;
  for (int i : slice) {
    slice_map.at(i) = idx;
    ++idx;
  }


}

void LDAUpdates::loadSparseSerialized(const char* mem) {

  version = load_value<uint64_t>(mem);

  int V = load_value<int>(mem);
  int K = load_value<int>(mem);

  change_nvt.clear();
  change_nvt.resize(V * K, 0);
  for (int i = 0; i < V; ++i) {
    int len = load_value<int>(mem);
    for (int j = 0; j < len; ++j) {
      int top = load_value<int>(mem);
      int count = load_value<int>(mem);
      change_nvt[i * K + top] = count;
    }
  }

  change_nt.clear();
  change_nt.reserve(K);
  for (int i = 0; i < K; ++i) {
    int temp = load_value<int>(mem);
    change_nt.push_back(temp);
  }

  slice.clear();
  slice.reserve(V);
  for (int i = 0; i < V; ++i) {
    int temp = load_value<int>(mem);
    slice.push_back(temp);
  }

  update_bucket = load_value<uint32_t>(mem);

  int idx = 0;
  for (int i : slice) {
    slice_map.at(i) = idx;
    ++idx;
  }
}

std::shared_ptr<char> LDAUpdates::serialize(uint32_t* serialize_size) {
  *serialize_size =
      sizeof(uint64_t) +
      sizeof(int) * (4 + change_nvt.size() + change_nt.size() + slice.size());
  std::shared_ptr<char> mem_begin = std::shared_ptr<char>(
      new char[*serialize_size], std::default_delete<char[]>());
  char* mem = mem_begin.get();

  store_value<uint64_t>(mem, version);
  store_value<int>(mem, change_nvt.size());
  int* data = reinterpret_cast<int*>(mem);
  std::copy(change_nvt.begin(), change_nvt.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * change_nvt.size()));

  store_value<uint32_t>(mem, change_nt.size());
  data = reinterpret_cast<int*>(mem);
  std::copy(change_nt.begin(), change_nt.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * change_nt.size()));

  store_value<uint32_t>(mem, slice.size());
  data = reinterpret_cast<int*>(mem);
  std::copy(slice.begin(), slice.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * slice.size()));

  store_value<uint32_t>(mem, update_bucket);

  return mem_begin;
}

std::shared_ptr<char> LDAUpdates::serialize_sparse(uint32_t* serialize_size) {

  std::vector<std::vector<std::pair<int, int>>> sparse_change_nvt;
  sparse_change_nvt.reserve(slice.size());

  int N = 0, K = change_nt.size();
  for (int i = 0; i < slice.size(); ++i) {
    std::vector<std::pair<int, int>> sparse_change_nt_vi;
    for (int j = 0; j < K; ++ j){
      if (change_nvt[i * K + j] != 0) {
        N += 1;
        sparse_change_nt_vi.push_back(std::make_pair(j, change_nvt[i * K + j]));
      }
    }
    sparse_change_nvt.push_back(sparse_change_nt_vi);
  }

  *serialize_size =
      sizeof(uint64_t) +
      sizeof(int) * (3 + 2 * N + change_nt.size() + 2 * slice.size());
  std::shared_ptr<char> mem_begin = std::shared_ptr<char>(
      new char[*serialize_size], std::default_delete<char[]>());
  char* mem = mem_begin.get();

  store_value<uint64_t>(mem, version);
  store_value<int>(mem, slice.size());
  store_value<uint32_t>(mem, change_nt.size());

  for (int i = 0; i < slice.size(); ++i) {
    store_value<int>(mem, sparse_change_nvt[i].size());
    for (auto& temp: sparse_change_nvt[i]) {
      store_value<int>(mem, temp.first);
      store_value<int>(mem, temp.second);
    }
  }

  int* data = reinterpret_cast<int*>(mem);
  std::copy(change_nt.begin(), change_nt.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * change_nt.size()));

  // store_value<uint32_t>(mem, slice.size());
  data = reinterpret_cast<int*>(mem);
  std::copy(slice.begin(), slice.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * slice.size()));

  store_value<uint32_t>(mem, update_bucket);

  return mem_begin;
}

uint64_t LDAUpdates::getSerializedSize() const {
  return sizeof(uint64_t) + // version
         sizeof(uint32_t) * // store the size and all entries for all variables
             (4 + change_nvt.size() + change_nt.size() + slice.size());
}

void LDAUpdates::print() const {
  std::cout << "Printing LDAUpdates. version: " << version << std::endl;
  for (const auto& v : change_nvt) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  for (const auto& v : change_nt) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  for (const auto& v : slice) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
}

int LDAUpdates::update(const LDAUpdates& gradient, std::vector<int>& vocabs_to_update) {

  int K = change_nt.size();
  for (int i = 0; i < gradient.slice.size(); ++i) {
    for (int j = 0; j < K; ++j) {
      // change_nvt[slice_map[gradient.slice[i]] * K + j] +=
      //     gradient.change_nvt[i * K + j];
      change_nvt[slice_map.at(gradient.slice[i]) * K + j] +=
          gradient.change_nvt[i * K + j];
    }
  }

  for (int i = 0; i < change_nt.size(); ++i) {
    change_nt[i] += gradient.change_nt[i];
  }

  vocabs_to_update = std::vector<int>(gradient.slice.begin(), gradient.slice.end());

  return gradient.update_bucket;
}

void LDAUpdates::get_partial_nvt(std::vector<int>& nvt, std::vector<int>& local_slice) {

  int K = change_nt.size();

  for (auto& word_idx: local_slice) {
    nvt.insert(nvt.end(),
               change_nvt.begin() + slice_map.at(word_idx) * K,
               change_nvt.begin() + (slice_map.at(word_idx) + 1) * K);


    // std::cout << nvt.size() << std::endl;
  }
}

void LDAUpdates::get_partial_sparse_nvt(
                std::vector<std::vector<int>>& nvt_sparse,
                std::vector<int>& local_slice){
  int K = change_nt.size();
  for (auto& word_idx: local_slice) {
    std::vector<int> nt_sparse_vi;
    nt_sparse_vi.reserve(K);
    for (int j = 0; j < K; ++j) {
      if (change_nvt[slice_map.at(word_idx) * K + j] > 0) {
        nt_sparse_vi.push_back(change_nvt[slice_map.at(word_idx) * K + j]);
      }
    }
    nvt_sparse.push_back(nt_sparse_vi);
  }
}

char* LDAUpdates::get_partial_model(const char* s, uint32_t& to_send_size) {

  // slice_map.clear();
  // int idx = 0;
  // for (int i : slice) {
  //   slice_map.insert(std::make_pair(i, idx));
  //   ++idx;
  // }

  // std::vector<int> partial_nvt;
  std::vector<std::vector<std::pair<int, int>>> sparse_partial_nvt;
  // std::vector<int> slice_s;
  int N = 0, word_idx;

  int K = change_nt.size();

  int len = load_value<int>(s);
  for (int i = 0; i < len; ++i) {

    word_idx = load_value<int>(s);
    // slice_s.push_back(word_idx);

    std::vector<std::pair<int, int>> sparse_nt_vi;

    for (int j = 0; j < K; ++j) {
      if (change_nvt[slice_map.at(word_idx) + j] > 0) {
        sparse_nt_vi.push_back(std::make_pair(j, change_nvt[slice_map.at(word_idx) * K + j]));
        N += 1;
      }
    }

    sparse_partial_nvt.push_back(sparse_nt_vi);
    // partial_nvt.insert(partial_nvt.end(),
    //                    change_nvt.begin() + slice_map[word_idx] * K,
    //                    change_nvt.begin() + (slice_map[word_idx] + 1) * K);
  }

  // for (int j = 0; j < K; ++j) {
  //   std::cout << change_nvt[slice_map.at(word_idx) + j] << " ";
  // }
  // std::cout << std::endl;
  //
  // std::cout << N << " " << len * K << std::endl;

  // to_send_size = sizeof(uint32_t) * (1 + partial_nvt.size() + change_nt.size());
  to_send_size = sizeof(uint32_t) * (1 + len + N * 2 + K);

  char* mem = new char[to_send_size];
  char* mem_begin = mem;

  store_value<uint32_t>(mem, len);
  for (int i = 0; i < len; ++i) {
    store_value<uint32_t>(mem, sparse_partial_nvt[i].size());
    for (auto& a : sparse_partial_nvt[i]) {
      store_value<uint32_t>(mem, a.first);
      store_value<uint32_t>(mem, a.second);
    }
  }

  // store_value<uint32_t>(mem, partial_nvt.size());
  // uint32_t* data = reinterpret_cast<uint32_t*>(mem);
  // std::copy(partial_nvt.begin(), partial_nvt.end(), data);
  // mem = reinterpret_cast<char*>(
  //     (reinterpret_cast<char*>(mem) + sizeof(uint32_t) * partial_nvt.size()));

  uint32_t* data = reinterpret_cast<uint32_t*>(mem);
  std::copy(change_nt.begin(), change_nt.end(), data);

  return mem_begin;

}

}  // namespace cirrus
