#include <ModelGradient.h>
#include <iostream>
#include <algorithm>
#include <Utils.h>
#include <cassert>
#include "Constants.h"
#include <cstring>
#include <set>
#include "lz4.h"

// #include "temp.h"
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
  // change_nvt = std::move(other.change_nvt);
  // change_nt = std::move(other.change_nt);
  // slice = std::move(other.slice);
  // version = other.version;
  change_nvt_ptr = other.change_nvt_ptr;
  change_nt_ptr = other.change_nt_ptr;
  slice = std::move(other.slice);
  version = other.version;
  sparse_records.fill(-1);
}

// LDAUpdates::LDAUpdates(const std::vector<int>& nvt, const std::vector<int>& nt)
//     : change_nvt(nvt), change_nt(nt) {
//   slice.clear();
// }
LDAUpdates::LDAUpdates(const std::vector<int>& nvt, const std::vector<int>& nt) {
  change_nvt_ptr.reset(new std::vector<int>(nvt));
  change_nt_ptr.reset(new std::vector<int>(nt));
  slice.clear();
  sparse_records.fill(-1);
}

LDAUpdates::LDAUpdates(const std::vector<int>& nvt,
                       const std::vector<int>& nt,
                       const std::vector<int>& s)
    : slice(s) {
  change_nvt_ptr.reset(new std::vector<int>(nvt));
  change_nt_ptr.reset(new std::vector<int>(nt));
  sparse_records.fill(-1);
}


LDAUpdates::LDAUpdates(const std::vector<int>& nvt,
                       const std::vector<int>& nt,
                       const std::vector<int>& s,
                       int to_update)
    : slice(s), update_bucket(to_update) {
  change_nvt_ptr.reset(new std::vector<int>(nvt));
  change_nt_ptr.reset(new std::vector<int>(nt));
  sparse_records.fill(-1);
}

LDAUpdates::LDAUpdates(int to_update) : update_bucket(to_update) {
  slice.clear();
  change_nvt_ptr->clear();
  change_nt_ptr->clear();
  sparse_records.fill(-1);
}

LDAUpdates::LDAUpdates(int nvt_dim, int nt_dim, int slice_size) {
  change_nvt_ptr->resize(nvt_dim);
  change_nt_ptr->resize(nt_dim);
  slice.resize(slice_size);
  version = 0;
  sparse_records.fill(-1);
}

LDAUpdates& LDAUpdates::operator=(LDAUpdates&& other) {
  change_nvt_ptr = other.change_nvt_ptr;
  change_nt_ptr = other.change_nt_ptr;
  slice = std::move(other.slice);
  version = other.version;
  sparse_records.fill(-1);
  return *this;
}

void LDAUpdates::loadSerialized(const char* mem) {

  version = load_value<uint64_t>(mem);

  int len = load_value<uint32_t>(mem);
  int K = load_value<uint32_t>(mem);
  int V = len / K;

  sparse_nvt_indices.reserve(V);
  temp_look_up.fill(-1);
  sparse_look_up.fill(-1);

  // std::cout << "K: " << K << " V: " << V << std::endl;

  change_nvt_ptr.reset(new std::vector<int>());
  // change_nvt_ptr->clear();

  std::cout << len << std::endl;
  change_nvt_ptr->reserve(len);
  // change_nvt_indices.reserve(V);

  // int top = 0;
  // std::vector<int> nz_indices;
  // nz_indices.reserve(K);

  for (int i = 0; i < len; ++i) {
    // top += 1;
    // if (top == K) {
    //   top = 0;
    //   nz_indices.push_back(nz_indices);
    //   nz_indices.clear();
    //   nz_indices.reserve(K);
    // }
    int temp = load_value<int>(mem);
    change_nvt_ptr->push_back(temp);

    // if (temp != 0) {
    //   nz_indices.push_back(top);
    // }
  }

  change_nt_ptr.reset(new std::vector<int>());
  change_nt_ptr->clear();
  change_nt_ptr->reserve(K);
  for (int i = 0; i < K; ++i) {
    int temp = load_value<int>(mem);
    change_nt_ptr->push_back(temp);
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

  // std::cout << " slice size: " << slice.size() << std::endl;

  sparse_records.fill(-1);

}

void LDAUpdates::loadSparseSerialized(const char* mem) {

  version = load_value<uint64_t>(mem);

  int V = load_value<int>(mem);
  int K = load_value<int>(mem);

  sparse_change_nvt_ptr.reset(new std::vector<std::vector<std::pair<int, int>>>());
  sparse_change_nvt_ptr->reserve(V);

  for (int i = 0; i < V; ++i) {
    int len = load_value<int>(mem);

    std::vector<std::pair<int, int>> sparse_change_nt_vi;
    sparse_change_nt_vi.reserve(len);

    for (int j = 0; j < len; ++j) {
      int16_t top = load_value<int16_t>(mem);
      int16_t count = load_value<int16_t>(mem);
      // change_nvt[i * K + top] = count;
      // change_nvt_ptr->operator[](i * K + top) = count;
      sparse_change_nt_vi.push_back(std::make_pair(top, count));
    }
    sparse_change_nvt_ptr->push_back(sparse_change_nt_vi);
  }

  change_nt_ptr.reset(new std::vector<int>());
  change_nt_ptr->reserve(K);

  for (int i = 0; i < K; ++i) {
    int temp = load_value<int>(mem);
    change_nt_ptr->push_back(temp);
  }

  slice.clear();
  slice.reserve(V);
  for (int i = 0; i < V; ++i) {
    int temp = load_value<int>(mem);
    slice.push_back(temp);
    // std::cout << temp << " ";
  }
  // std::cout << std::endl;

  update_bucket = load_value<uint32_t>(mem);

  slice_map.fill(-1);
  int idx = 0;
  for (int i : slice) {
    slice_map.at(i) = idx;
    ++idx;
  }
}

std::shared_ptr<char> LDAUpdates::serialize(uint32_t* serialize_size) {
  *serialize_size =
      sizeof(uint64_t) +
      sizeof(int) * (4 + change_nvt_ptr->size() + change_nt_ptr->size() + slice.size());
  std::shared_ptr<char> mem_begin = std::shared_ptr<char>(
      new char[*serialize_size], std::default_delete<char[]>());
  char* mem = mem_begin.get();

  store_value<uint64_t>(mem, version);
  store_value<int>(mem, change_nvt_ptr->size());
  store_value<uint32_t>(mem, change_nt_ptr->size());

  int* data = reinterpret_cast<int*>(mem);
  std::copy(change_nvt_ptr->begin(), change_nvt_ptr->end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * change_nvt_ptr->size()));

  // store_value<uint32_t>(mem, change_nt_ptr->size());
  data = reinterpret_cast<int*>(mem);
  std::copy(change_nt_ptr->begin(), change_nt_ptr->end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * change_nt_ptr->size()));

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

  int N = 0, K = change_nt_ptr->size();
  for (int i = 0; i < slice.size(); ++i) {
    std::vector<std::pair<int, int>> sparse_change_nt_vi;
    for (int j = 0; j < K; ++ j){
      // if (change_nvt[i * K + j] != 0) {
      //   N += 1;
      //   sparse_change_nt_vi.push_back(std::make_pair(j, change_nvt[i * K + j]));
      // }
      if (change_nvt_ptr->operator[](i * K + j) != 0) {
        N += 1;
        sparse_change_nt_vi.push_back(std::make_pair(j, change_nvt_ptr->operator[](i * K + j)));
      }
    }
    sparse_change_nvt.push_back(sparse_change_nt_vi);
  }

  *serialize_size =
      sizeof(uint64_t) +
      sizeof(int) * (3  + change_nt_ptr->size() + 2 * slice.size()) +
      sizeof(int16_t) * (2 * N);
  std::shared_ptr<char> mem_begin = std::shared_ptr<char>(
      new char[*serialize_size], std::default_delete<char[]>());
  char* mem = mem_begin.get();

  store_value<uint64_t>(mem, version);
  store_value<int>(mem, slice.size());
  store_value<uint32_t>(mem, change_nt_ptr->size());

  for (int i = 0; i < slice.size(); ++i) {
    store_value<int>(mem, sparse_change_nvt[i].size());
    for (auto& temp: sparse_change_nvt[i]) {
      store_value<int16_t>(mem, temp.first);
      store_value<int16_t>(mem, temp.second);
    }
  }

  int* data = reinterpret_cast<int*>(mem);
  std::copy(change_nt_ptr->begin(), change_nt_ptr->end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * change_nt_ptr->size()));

  // store_value<uint32_t>(mem, slice.size());
  data = reinterpret_cast<int*>(mem);
  std::copy(slice.begin(), slice.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * slice.size()));

  // for (int i = 0; i < slice.size(); ++i) {
    // std::cout << slice[i] << " ";
  // }
  // std::cout << std::endl;

  store_value<uint32_t>(mem, update_bucket);

  return mem_begin;
}

uint64_t LDAUpdates::getSerializedSize() const {
  return sizeof(uint64_t) + // version
         sizeof(uint32_t) * // store the size and all entries for all variables
             (4 + change_nvt_ptr->size() + change_nt_ptr->size() + slice.size());
}

void LDAUpdates::print() const {
  std::cout << "Printing LDAUpdates. version: " << version << std::endl;
  // for (const auto& v : change_nvt_ptr.get()) {
  //   std::cout << v << " ";
  // }
  // std::cout << std::endl;
  //
  // for (const auto& v : change_nt_ptr.get()) {
  //   std::cout << v << " ";
  // }
  // std::cout << std::endl;
  //
  // for (const auto& v : slice) {
  //   std::cout << v << " ";
  // }
  std::cout << std::endl;
}

int LDAUpdates::update(const LDAUpdates& gradient, std::vector<int>& vocabs_to_update) {

  // std::cout << "aa\n";
  int K = change_nt_ptr->size();

  // int bb = 0;

  // std::cout << "pre: \n";
  // for (int i = 0; i < K; ++i) {
  //   bb += change_nvt_ptr->operator[](slice_map.at(gradient.slice[100]) * K + i);
  //   std::cout << change_nvt_ptr->operator[](slice_map.at(gradient.slice[100]) * K + i) << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "pre count: " << bb << " !!\n";
  //
  // std::vector<int> temp(K, 0);
  // for (auto& a: gradient.sparse_change_nvt_ptr->operator[](100)) {
  //   temp[a.first] = a.second;
  // }
  // std::cout << "update: \n";
  // for (int i = 0; i < K; ++i) {
  //   std::cout << temp[i] << " ";
  // }
  // std::cout << std::endl;

  for (int i = 0; i < gradient.slice.size(); ++i) {

    // if (temp_look_up[gradient.slice[i]] != -1) {
    //   std::cout << "----------\n";
    //   std::cout << "pre sparse indices: \n";
    //   for ( int j = 0; j < sparse_nvt_indices[temp_look_up[gradient.slice[i]]].size(); ++j) {
    //     std::cout << sparse_nvt_indices[temp_look_up[gradient.slice[i]]][j] << " ";
    //   }
    //   std::cout << std::endl;
    //   std::cout << "pre length: " << sparse_records[gradient.slice[i]] << std::endl;
    // }


    for (auto& a: gradient.sparse_change_nvt_ptr->operator[](i)) {

      // int temp = change_nvt_ptr->operator[](slice_map.at(gradient.slice[i]) * K + a.first);

      if (temp_look_up[gradient.slice[i]] != -1 && change_nvt_ptr->operator[](slice_map.at(gradient.slice[i]) * K + a.first) == 0) {

        // std::cout << "top: " << a.first << " update count: " << a.second << std::endl;
        // sparse_nvt_indices[temp_look_up[gradient.slice[i]]].push_back(a.first);
        sparse_nvt_indices[temp_look_up[gradient.slice[i]]].insert(a.first);
        if (sparse_records[gradient.slice[i]] == -1) {
          throw std::runtime_error("Error in store type");
        }
        // std::set<int> temp_set(sparse_nvt_indices[temp_look_up[gradient.slice[i]]].begin(),
        //                        sparse_nvt_indices[temp_look_up[gradient.slice[i]]].end());
        // sparse_nvt_indices[temp_look_up[gradient.slice[i]]] = std::vector<int>(temp_set.begin(), temp_set.end());
        sparse_records[gradient.slice[i]] = sparse_nvt_indices[temp_look_up[gradient.slice[i]]].size();
      }

      change_nvt_ptr->operator[](slice_map.at(gradient.slice[i]) * K + a.first) += a.second;

      if (temp_look_up[gradient.slice[i]] != -1 && change_nvt_ptr->operator[](slice_map.at(gradient.slice[i]) * K + a.first) == 0) {
        // std::remove(sparse_nvt_indices[temp_look_up[gradient.slice[i]]].begin(),
        //             sparse_nvt_indices[temp_look_up[gradient.slice[i]]].end(),
        //             a.first);
        sparse_nvt_indices[temp_look_up[gradient.slice[i]]].erase(a.first);
        if (sparse_records[gradient.slice[i]] == -1) {
          throw std::runtime_error("Error in store type");
        }
        // std::set<int> temp_set(sparse_nvt_indices[temp_look_up[gradient.slice[i]]].begin(),
        //                        sparse_nvt_indices[temp_look_up[gradient.slice[i]]].end());
        // sparse_nvt_indices[temp_look_up[gradient.slice[i]]] = std::vector<int>(temp_set.begin(), temp_set.end());
        sparse_records[gradient.slice[i]] = sparse_nvt_indices[temp_look_up[gradient.slice[i]]].size();
      }

      // if (temp == 0) {
      //   change_nvt_indices[slice_map.at(gradient.slice[i])].push_back(a.first);
      // }
      //
      // if (temp + a.second <= 0) {
      //   change_nvt_indices[slice_map.at(gradient.slice[i])].push_back(a.first);
      // }


    }
    // for (int j = 0; j < gradient.sparse_change_nvt_ptr->operator[](i); ++j) {
    //   // change_nvt[slice_map[gradient.slice[i]] * K + j] +=
    //   //     gradient.change_nvt[i * K + j];
    //   // change_nvt[slice_map.at(gradient.slice[i]) * K + j] +=
    //   //     gradient.change_nvt[i * K + j];
    //
    //   change_nvt_ptr->operator[](slice_map.at(gradient.slice[i]) * K + j) +=
    //       gradient.change_nvt_ptr->operator[](i * K + j);
    //
    // }

    // std::cout << "nvt row: \n";
    // for (int j = 0; j < K; ++j) {
    //   std::cout << change_nvt_ptr->operator[](slice_map.at(gradient.slice[i]) * K + j) << " ";
    // }
    // std::cout << std::endl;
    //
    // if (temp_look_up[gradient.slice[i]] != -1) {
    //   std::cout << "----------\n";
    //   std::cout << "after sparse indices: \n";
    //   for ( int j = 0; j < sparse_nvt_indices[temp_look_up[gradient.slice[i]]].size(); ++j) {
    //     std::cout << sparse_nvt_indices[temp_look_up[gradient.slice[i]]][j] << " ";
    //   }
    //   std::cout << std::endl;
    //   std::cout << "after length: " << sparse_records[gradient.slice[i]] << std::endl;
    //
    // }
  }

  for (int i = 0; i < change_nt_ptr->size(); ++i) {
    // change_nt[i] += gradient.change_nt[i];
    change_nt_ptr->operator[](i) += gradient.change_nt_ptr->operator[](i);

  }

  vocabs_to_update = std::vector<int>(gradient.slice.begin(), gradient.slice.end());

  // std::cout << "bb\n";

  // std::cout << "after: \n";
  // bb = 0;
  // for (int i = 0; i < K; ++i) {
  //   bb += change_nvt_ptr->operator[](slice_map.at(gradient.slice[100]) * K + i);
  //   std::cout << change_nvt_ptr->operator[](slice_map.at(gradient.slice[100]) * K + i) << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "after count: " << bb << " !!\n";
  //
  // std::cout << "nt: \n";
  // for (int i = 0; i < K; ++i) {
  //   std::cout << change_nt_ptr->operator[](i) << " ";
  // }
  // std::cout << std::endl;


  return gradient.update_bucket;
}

// void LDAUpdates::get_partial_nvt(std::vector<int>& nvt, std::vector<int>& local_slice) {
//
//   int K = change_nt_ptr->size();
//
//   for (auto& word_idx: local_slice) {
//     nvt.insert(nvt.end(),
//                change_nvt_ptr->begin() + slice_map.at(word_idx) * K,
//                change_nvt_ptr->begin() + (slice_map.at(word_idx) + 1) * K);
//
//
//     // std::cout << nvt.size() << std::endl;
//   }
// }

// void LDAUpdates::get_partial_sparse_nvt(
//                 std::vector<std::vector<int>>& nvt_sparse,
//                 std::vector<int>& local_slice){
//   int K = change_nt_ptr->size();
//   for (auto& word_idx: local_slice) {
//     std::vector<int> nt_sparse_vi;
//     nt_sparse_vi.reserve(K);
//     for (int j = 0; j < K; ++j) {
//       if (change_nvt[slice_map.at(word_idx) * K + j] > 0) {
//         nt_sparse_vi.push_back(change_nvt[slice_map.at(word_idx) * K + j]);
//       }
//     }
//     nvt_sparse.push_back(nt_sparse_vi);
//   }
// }

void LDAUpdates::get_partial_sparse_nvt_ptr(
                    std::shared_ptr<std::vector<std::vector<int>>>& nvt_ptr,
                    const std::vector<int>& local_slice) {
  std::vector<int> nvt_sparse;
  int K = change_nt_ptr->size();
  for (auto& word_idx: local_slice) {
    std::vector<int> nt_sparse_vi;
    nt_sparse_vi.reserve(K);
    for (int j = 0; j < K; ++j) {
      // if (change_nvt[slice_map.at(word_idx) * K + j] > 0) {
      //   nt_sparse_vi.push_back(change_nvt[slice_map.at(word_idx) * K + j]);
      // }
      if (change_nvt_ptr->operator[](slice_map.at(word_idx) * K + j) > 0) {
        nt_sparse_vi.push_back(
          change_nvt_ptr->operator[](slice_map.at(word_idx) * K + j)
        );
      }
    }
    nvt_ptr->push_back(nt_sparse_vi);
  }
}


char* LDAUpdates::get_partial_model(int slice_id, uint32_t& to_send_size, uint32_t& uncompressed_size, bool check_sparse) {

  auto ttt = get_time_ms();

  int N = 0, S = 0, word_idx;
  int K = change_nt_ptr->size();

  // int32_t len = load_value<int32_t>(s);
  int len = fixed_slices[slice_id].size();

  int sparse_type = 1, dense_type = 2;

  // the worst case where everything is stored with dense structure
  int temp_counts = (2 + 2 * len + len * K + K);
  int temp_size = sizeof(uint32_t) * temp_counts;

  auto start_time_benchmark = get_time_ms();
  // auto start_time_temp = get_time_ms();
  // auto start_time_ttemp = get_time_ms();
  counts += 1;

  char* mem = new char[temp_size];
  char* mem_begin = mem;
  store_value<int32_t>(mem, global_v);
  store_value<int32_t>(mem, len);

  // std::cout << slice.size() << std::endl;
  // std::cout << len << std::endl;

  // std::cout << word_idx << " ";

  // std::vector<std::vector<std::pair<int, int>>> sparse_nvt;
  // sparse_nvt.reserve(len);

  // std::array<int, 1000000> temp_look_up;
  // temp_look_up.fill(-1);
  // int temp_counter = 0;

  // std::cout << slice_map.size() << " !!!" << std::endl;

  auto start_time_temp = get_time_ms();
  for (int i = 0; i < len; ++i) {

    auto start_time_ttemp = get_time_ms();
    word_idx = fixed_slices[slice_id][i];

    std::set<int> sparse_nt_vi;
    // sparse_nt_vi.reserve(K);

    int n = 0;

    // auto s_time = get_time_ms();
    if (sparse_records[word_idx] == -1) {
      for (int j = 0; j < K; ++j) {
        // int entry = change_nvt_ptr->operator[](slice_map[word_idx] * K + j);
        // if (entry > 0) {
        //   sparse_nt_vi.push_back(std::make_pair(j, entry));
        //   n += 1;
        // }
        if (change_nvt_ptr->operator[](slice_map[word_idx] * K + j) != 0) {
          // sparse_nt_vi.push_back(std::make_pair(j, change_nvt_ptr->operator[](slice_map[word_idx] * K + j)));
          // sparse_nt_vi.push_back(j);
          sparse_nt_vi.insert(j);
          n += 1;
        }

        // if (sparse_look_up[word_idx] > 0) {
        //   // sparse_nvt[temp_look_up[word_idx]] = sparse_nt_vi;
        //   // sparse_records[word_idx] = n;
        //   // n = 0;
        //   if (sparse_look_up[word_idx] == 20) {
        //     sparse_look_up[word_idx] = -2;
        //   }
        // }
      }
    }

    time_check_sparse += (get_time_ms() - start_time_ttemp) / 1000.0;
    // time_temp += (get_time_ms() - s_time) / 1000.0;

    // if 2 * n < K, then adopting sparse structure would help
    // reduce the size of partial model
    //
    // if sparse_records[word_idx] != -1, n would always be 0
    // so that sparse data structure would be used
    if (2 * n < K || sparse_records[word_idx] != -1) {
      // 1 -> sparse structure

      start_time_ttemp = get_time_ms();
      store_value<int16_t>(mem, sparse_type);
      // std::cout << sparse_type << std::endl;

      if (n == 0) {
        if (sparse_records[word_idx] == -1) {
          throw std::runtime_error("Error in store type");
        }
        if (temp_look_up[word_idx] == -1) {
          // std::cout << word_idx << " " << temp_look_up[word_idx] << " ***\n";
          throw std::runtime_error("Error in store previous sparse");
        }
        // if (sparse_look_up[word_idx] == -1) {
        //   // std::cout << word_idx << " " << temp_look_up[word_idx] << " ***\n";
        //   throw std::runtime_error("Error in sparse_look_up");
        // }
        store_value<int16_t>(mem, sparse_records[word_idx]);

        for (auto& a: sparse_nvt_indices[temp_look_up[word_idx]]) {
          store_value<int16_t>(mem, a);
          store_value<int16_t>(mem, change_nvt_ptr->operator[](slice_map[word_idx] * K + a));
        }

        N += sparse_records[word_idx];

        // if (sparse_look_up[word_idx] != -2) {
        //   sparse_look_up[word_idx] += 1;
        // }

      } else {
        store_value<int16_t>(mem, n);
        sparse_records[word_idx] = n;

        sparse_nvt_indices.push_back(sparse_nt_vi);
        temp_look_up[word_idx] = temp_counter;
        temp_counter += 1;

        // std::cout << word_idx << " " << temp_counter << " " << temp_look_up[word_idx] << std::endl;
        // std::cout << n << " " << sparse_nt_vi.size() << std::endl;
        for (auto& a: sparse_nt_vi) {
          store_value<int16_t>(mem, a);
          store_value<int16_t>(mem, change_nvt_ptr->operator[](slice_map[word_idx] * K + a));
        }

        // sparse_look_up[word_idx] = 1;

        N += n; // # of <top, count> pairs stored with sparse structure
      }

      S += 1; // # of words that are stored with sparse structure

      time_serial_sparse += (get_time_ms() - start_time_ttemp) / 1000.0;

    } else {
      // std::cout << change_nvt_ptr->size() << " " << slice_map.at(word_idx) * K << " cc\n";
      // 2 -> dense structure

      start_time_ttemp = get_time_ms();

      // std::cout << dense_type << std::endl;
      store_value<int16_t>(mem, dense_type);
      for (int j = 0; j < K; ++j) {
        store_value<int16_t>(mem, change_nvt_ptr->operator[](slice_map[word_idx] * K + j));
        // std::cout << change_nvt_ptr->operator[](slice_map[word_idx] * K + j) << " ";
      }
      // std::cout << std::endl;
      // uint16_t* data = reinterpret_cast<uint16_t*>(mem);
      // std::copy(change_nvt_ptr->begin() + slice_map[word_idx] * K,
      //           change_nvt_ptr->begin() + (slice_map[word_idx] + 1) * K,
      //           data);
      // mem = reinterpret_cast<char*>((reinterpret_cast<char*>(mem) + sizeof(uint16_t) * K));

      time_ttemp += (get_time_ms() - start_time_ttemp) / 1000.0;
    }
  }


  time_nvt_find += (get_time_ms() - start_time_temp) / 1000.0;

  for (int i = 0; i < K; ++i) {
    store_value<int32_t>(mem, change_nt_ptr->operator[](i));
  }


  for (int i = 0; i < len; ++i) {
    store_value<int32_t>(mem, fixed_slices[slice_id][i]);
  }

  // int32_t* data = reinterpret_cast<int32_t*>(mem);
  // std::copy(change_nt_ptr->begin(), change_nt_ptr->end(), data);
  // mem = reinterpret_cast<char*>((reinterpret_cast<char*>(mem) + sizeof(int32_t) * change_nt_ptr->size()));
  //
  // time_find_partial += (get_time_ms() - start_time_temp) / 1000.0;
  // data = reinterpret_cast<int32_t*>(mem);
  // std::copy(fixed_slices[slice_id].begin(), fixed_slices[slice_id].end(), data);


  start_time_temp = get_time_ms();

  // uncompressed_size = sizeof(uint32_t) * (1 + K) + sizeof(uint8_t) * (len) + sizeof(uint16_t) * (S + 2 * N + (len - S) * K);
  // uncompressed_size = sizeof(int32_t) * (2 + K + len) + sizeof(uint16_t) * (len + S + 2 * N + (len - S) * K);
  uncompressed_size = sizeof(int32_t) * (2 + K + len) +
                      sizeof(int16_t) * (len + S + 2 * N + (len - S) * K);
                      // sizeof(int8_t) * len;
  size_t max_compressed_size = LZ4_compressBound(uncompressed_size);
  char* compressed_mem = new char[uncompressed_size];

  time_temp += (get_time_ms() - start_time_temp) / 1000.0;

  to_send_size = LZ4_compress_default(mem_begin, compressed_mem, uncompressed_size, max_compressed_size);
  // std::cout << compressed_size << " ****\n";

  // codec.encodeArray(to_compress.data(), to_compress.size(), compressed.data(), compressedsize);
  // compressed.resize(compressedsize);
  // compressed.shrink_to_fit();

  time_compress += (get_time_ms() - start_time_temp) / 1000.0;

  total_time_to_compress += (get_time_ms() - start_time_temp) / 1000.;
  total_to_compress_size += uncompressed_size;
  total_compressed_size += to_send_size;

  // std::cout << uncompressed_size << " " << to_send_size << std::endl;

  double time_taken = (get_time_ms() - ttt) / 1000.0;

  delete mem_begin;

  // std::cout << to_send_size << " " << uncompressed_size << std::endl;

  // char* decompressed_mem = new char[uncompressed_size + 1024];
  // int decompressed_size = LZ4_decompress_fast(compressed_mem, decompressed_mem, compressed_size);
  //
  // std::cout << "orig: " << uncompressed_size << " now: " << decompressed_size << std::endl;
  // const char* buffer = decompressed_mem;
  // int tt = load_value<int>(buffer);
  // std::cout << "orig V: " << len << " now: " << tt << std::endl;

  // to_send_size = uncompressed_size;

  // std::cout << time_taken << " ****\n";
  // std::cout << std::strlen(mem_begin) << " (((())))\n";

  // std::cout << "orig: ";
  // for(int i=0; i<10; ++i) {
  //   std::cout << to_compress[i] << " ";
  // }
  // std::cout << std::endl;
  //
  // std::cout << "compressed: ";
  // for(int i=0; i<10; ++i) {
  //   std::cout << compressed[i] << " ";
  // }
  // std::cout << std::endl;
  // std::cout << "original size: " << to_compress.size() << std::endl;
  // std::cout << "receive_size size: " << compressed.size() << std::endl;

  time_whole += (get_time_ms() - start_time_benchmark) / 1000.0;
  // std::cout << "exit this funtion\n";
  return compressed_mem;

}

int LDAUpdates::pre_assign_slices(int slice_size) {

  int num_slices = slice.size() / slice_size;
  std::cout << "Global vocab dim: " << slice.size() << " slice_size: " << slice_size << " num_slices: " << num_slices << std::endl;

  fixed_slices.clear();
  fixed_slices.reserve(num_slices);
  int cur = 0;
  for (int i = 0; i < num_slices; ++i) {
    // fixed_slices[i].clear();
    std::vector<int> to_push;
    if (slice.size() - cur <= slice_size) {
      // fixed_slices[i].resize(slice.size() - cur);
      // std::copy(slice.begin() + cur, slice.end(), fixed_slices[i].begin());
      break;
    } else {
      // fixed_slices[i].resize(slice_size);
      to_push.resize(slice_size);
      std::copy(slice.begin() + cur, slice.begin() + cur + slice_size, to_push.begin());
      cur += slice_size;
      fixed_slices.push_back(to_push);
    }
  }
  global_v = cur;
  return fixed_slices.size();
}

}  // namespace cirrus
