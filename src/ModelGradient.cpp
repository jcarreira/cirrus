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

/**
 * Given a pointer to memory, method will serialize gradient into 'parts' number of parts
 * Method returns vector of size parts containing tuples of <a, b> such that for the ith tuple mem + a
 * is the start of the ith serialized gradient and mem + a + b is the end of that serialized gradient
 *
 */

std::vector<std::tuple<int, int>> LRSparseGradient::shard_serialize(
    void* mem,
    uint32_t parts) const {



  std::vector<int> starts(parts, 0); // First will store how many tuples per shard, ie starts[i] is the number
                                     // of gradient weights going to server number i
  std::vector<std::tuple<int, int>> starts_out(parts); // starts_out[i] stores <starting_offset(bytes), size(bytes)> of where the ith gradient 
                                                       // serialize starts and its size relative to mem.
  // TODO: Change to murmurhash
  std::hash<uint32_t> hash;  // hash function
  
  // Perform count
  for (const auto& w : weights) {
    starts[hash(w.first) % parts]++;
  }

  // starts[i] now will store how many weights into mem does the ith gradient serialzation start.
  
  int count = starts[0]; // how many weights belong in the current shard serialization
  int count_next = starts[1]; // how many weights belong in the next shard serialization
  
  starts[0] = 0;  // First shard serialization starts at 0
  starts[1] = count; // Next shard serialiation begins count weights after the begining of mem.

  for (int i = 0; i < parts; i++) {
    if (i != (parts - 1) and i != 0) {
      count_next = starts[i + 1];
      starts[i + 1] = starts[i] + count;
    }
    
    // How many [version(int), num_weights] + [number of (int, FEATURE_TYPEs)]
    // that lie previous
    uint64_t offset_from_mem_start = (i * (2 * sizeof(int))) +
                      (starts[i] * (sizeof(int) + sizeof(FEATURE_TYPE)));
    uint64_t shard_serialized_size = count * (sizeof(int) + sizeof(FEATURE_TYPE)) + 2 * sizeof(int); // Size in bytes of the ith shard
    // Shorten this. Makes a tuple out of position + size
    starts_out[i] = std::make_tuple(offset_from_mem_start, shard_serialized_size);
    
    // Write in the version and count
    put_value<int>(mem, version, offset_from_mem_start);
    put_value<int>(mem, count, offset_from_mem_start + sizeof(int));

    count = count_next;
  }

  // starts[i] will now designate how many weights in should we write in the next weight of gradient serialization i
  for (const auto& w : weights) {
    
    int index = w.first;
    FEATURE_TYPE weight = w.second;
    
    // TODO: Fix to murmur hash
    int ps_num = hash(index) % parts;
    int position = starts[ps_num];

    // Determine the offset from mem start to write the index and weight
    uint64_t offset =
        ((ps_num + 1) *
         (sizeof(int) + sizeof(int))) +  // Number of (version, count) variables
        position * (sizeof(int) + sizeof(FEATURE_TYPE));  //
    put_value<uint32_t>(mem, index, offset);
    put_value<FEATURE_TYPE>(mem, weight, offset + sizeof(uint32_t));

    // Update starts, since a new idx and weight was just written in
    starts[ps_num]++;
  }

  return starts_out;
}

uint64_t LRSparseGradient::getSerializedSize() const {
  return weights.size() * (sizeof(FEATURE_TYPE) + sizeof(int)) + // pairs (index, weight value)
    sizeof(int) * 2; // version + number of weights
}

uint64_t LRSparseGradient::getShardSerializedSize(int num_shards) const {
  return weights.size() * (sizeof(FEATURE_TYPE) +
                           sizeof(int)) +  // pairs (index, weight value)
         sizeof(int) * 2 * num_shards;     // version + number of weights
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

uint64_t MFSparseGradient::getShardSerializedSize(int num_shards) const {
  return sizeof(uint32_t) * (2 + 2) * num_shards +                          // num_users, num_items, 2x magic_values
         users_bias_grad.size() * (sizeof(int) + sizeof(FEATURE_TYPE)) +    // pairs (index, weight value)
         items_bias_grad.size() * (sizeof(int) + sizeof(FEATURE_TYPE)) +    // pairs (index, weight value)
         users_weights_grad.size() *
             (sizeof(int) + NUM_FACTORS * sizeof(FEATURE_TYPE)) +           // pairs (index, num_factors# weights)
         items_weights_grad.size() *
             (sizeof(int) + NUM_FACTORS * sizeof(FEATURE_TYPE));            // pairs (index, num_factors# weights)
}



std::vector<std::tuple<int, int>> MFSparseGradient::shard_serialize(
    void* mem,
    uint32_t minibatch_size,
    uint32_t num_ps) const {
 
  // TODO: use murmur hash
  std::hash<int> hashfunc;
  
  std::vector<int> starts(num_ps, 4 * sizeof(int)); // stores the size (bytes) per gradient shard.
  std::vector<int> icnts(num_ps, 0);  // icnts[i] stores the number of items of the ith gradient shard
  std::vector<int> ucnts(num_ps, 0);  // ucnts[i] stores the number of users of the ith gradient shard
  std::vector<std::tuple<int, int>> starts_out(num_ps);

  // Perform count of users
  uint32_t bias_grad_size = 2 * sizeof(int) + (NUM_FACTORS + 1) * sizeof(FEATURE_TYPE);
  for (const auto& user_bias : users_bias_grad) {
    int ps_num = (user_bias.first / (minibatch_size / num_ps)) % num_ps;
    starts[ps_num] += bias_grad_size;
    ucnts[ps_num]++;
  }
  
  // Perform count of items
  for (const auto& item_bias : items_bias_grad) {
    starts[hashfunc(item_bias.first) % num_ps] += bias_grad_size;
    icnts[hashfunc(item_bias.first) % num_ps]++;
  }

  // starts[i] = mem + starts[i] is where the next term of the ith gradient should be written
  int count = starts[0];  // stores size (bytes) of the current gradient shard
  int icnt = icnts[0];    // stores the number of items in the current gradient shard
  int ucnt = ucnts[0];    // stores the number of users in the current gradient shard

  starts[0] = 0;
  ucnts[0] = 0;
  icnts[0] = 0;
  
  int count_next = 0;
  int icnt_next = 0;
  int ucnt_next = 0;

  for (int i = 0; i < num_ps; i++) {
    // if i is not the last idx
    if (i != (num_ps - 1)) {
      count_next = starts[i + 1];
      icnt_next = icnts[i + 1];
      ucnt_next = ucnts[i + 1];

      starts[i + 1] = starts[i] + count;
      icnts[i + 1] = icnts[i] + icnt;
      ucnts[i + 1] = ucnts[i] + ucnt;
    }

    // Shorten this. Makes a tuple out of position + size
    starts_out[i] = std::make_tuple(starts[i], count);

    // How many [version(int), num_weights] + [number of (int, FEATURE_TYPEs)]
    // that lie previous
    uint64_t offset = starts[i];

	  // Insert Magic value and counts at the beginning of the gradient
    put_value<uint32_t>(mem, MAGIC_NUMBER, offset);
    put_value<int>(mem, ucnt, offset + sizeof(int));
    put_value<int>(mem, icnt, offset + 2 * sizeof(int));
    
    // Update starts[i] to the next available space
    starts[i] += 3 * sizeof(int);

    count = count_next;
    ucnt = ucnt_next;
    icnt = icnt_next;
  }

  // Serialization of the user bias
  // We assume a dense ordering of the users
  for (const auto& user_bias : users_bias_grad) {
    int index = user_bias.first;
    FEATURE_TYPE weight = user_bias.second;
    int ps_num = (user_bias.first / (minibatch_size / num_ps)) % num_ps;
    int converted_index =
        ((user_bias.first / minibatch_size)) * (minibatch_size / num_ps) +
        user_bias.first % (minibatch_size / num_ps);
    int position = starts[ps_num];
    put_value<int>(mem, converted_index, position);
    put_value<FEATURE_TYPE>(mem, weight, position + sizeof(int));
    starts[ps_num] += sizeof(int) + sizeof(FEATURE_TYPE);
  }

  // Serialization of the item bias
  for (const auto& bias_grad : items_bias_grad) {
    int index = bias_grad.first;
    FEATURE_TYPE v = bias_grad.second;
    int ps_num = hashfunc(index) % num_ps;
    int position = starts[ps_num];
    put_value<int>(mem, index, position);
    put_value<FEATURE_TYPE>(mem, v, position + sizeof(int));
    starts[ps_num] += sizeof(int) + sizeof(FEATURE_TYPE);
  }

  // Serialization of the user weights
  assert(users_weights_grad.size() == users_bias_grad.size());
  for (const auto& user : users_weights_grad) {
    int index = user.first;
    int converted_index =
        ((user.first / minibatch_size)) * (minibatch_size / num_ps) +
        user.first % (minibatch_size / num_ps);
    int ps_num = (user.first / (minibatch_size / num_ps)) % num_ps;
    int position = starts[ps_num];
    put_value<int>(mem, converted_index, position);
    starts[ps_num] += sizeof(int);
    assert(user.second.size() == NUM_FACTORS);
    for (const auto& weight_grad : user.second) {
      put_value<FEATURE_TYPE>(mem, weight_grad, starts[ps_num]);
      starts[ps_num] += sizeof(FEATURE_TYPE);
    }
  }

  // Serialization of the item weights
  assert(items_weights_grad.size() == items_bias_grad.size());
  for (const auto& item : items_weights_grad) {
    int index = item.first;
    int ps_num = hashfunc(index) % num_ps;
    int position = starts[ps_num];
    put_value<int>(mem, index, position);
    starts[ps_num] += sizeof(int);
    assert(item.second.size() == NUM_FACTORS);
    for (const auto& item_grad : item.second) {
      put_value<FEATURE_TYPE>(mem, item_grad, starts[ps_num]);
      starts[ps_num] += sizeof(FEATURE_TYPE);
    }
  }

  // Put the magic value in the back of every gradient
  for (int i = 0; i < num_ps; i++)
    put_value<uint32_t>(mem, 0x1338, starts[i]);

  return starts_out;
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
  store_value<uint32_t>(mem, 0x1338);  // magic value
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

} // namespace cirrus

