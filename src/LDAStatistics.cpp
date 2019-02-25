#include <LDAStatistics.h>
#include <Utils.h>
#include <string.h>
#include <iostream>

#include "Constants.h"

// #define MAX_MSG_SIZE (1024 * 1024 * 100)

namespace cirrus {
LDAStatistics::LDAStatistics() {}

LDAStatistics::LDAStatistics(int K,
                             std::vector<std::vector<int> >& ndt,
                             std::vector<int>& slice,
                             std::vector<int>& t,
                             std::vector<int>& d,
                             std::vector<int>& w) {
  K_ = K;
  ndt_ = ndt;
  slice_ = slice;
  t_ = t;
  d_ = d;
  w_ = w;
}

LDAStatistics::LDAStatistics(const char* msg) {
  K_ = load_value<int16_t>(msg);

  int32_t t_size = load_value<int32_t>(msg);
  t_.clear();
  d_.clear();
  w_.clear();
  t_.reserve(t_size);
  d_.reserve(t_size);
  w_.reserve(t_size);
  for (int32_t i = 0; i < t_size; ++i) {
    int16_t t = load_value<int16_t>(msg);
    t_.push_back(t);

    int32_t d = load_value<int32_t>(msg);
    d_.push_back(d);

    int32_t w = load_value<int32_t>(msg);
    w_.push_back(w);
  }

  int32_t num_docs = load_value<int32_t>(msg);
  ndt_.clear();
  ndt_.reserve(num_docs);
  std::vector<int> ndt_row;

  for (int i = 0; i < num_docs; ++i) {
    int8_t store_type = load_value<int8_t>(msg);
    ndt_row.clear();

    if (store_type == SPARSE) {
      ndt_row.resize(K_, 0);
      int16_t len = load_value<int16_t>(msg);
      for (int j = 0; j < len; ++j) {
        int16_t top = load_value<int16_t>(msg);
        int16_t count = load_value<int16_t>(msg);
        ndt_row[top] = count;
      }
    } else if (store_type == DENSE) {
      ndt_row.reserve(K_);
      for (int j = 0; j < K_; ++j) {
        int16_t temp = load_value<int16_t>(msg);
        ndt_row.push_back(temp);
      }
    }
    ndt_.push_back(ndt_row);
  }
  current = 0;
}

char* LDAStatistics::serialize(uint64_t& to_send_size) {
  char* msg = new char[get_serialize_size()];
  char* msg_begin = msg;  // need to keep this pointer to delete later

  store_value<int16_t>(msg, K_);

  store_value<int32_t>(msg, t_.size());
  for (int32_t i = 0; i < t_.size(); ++i) {
    store_value<int16_t>(msg, t_[i]);
    store_value<int32_t>(msg, d_[i]);
    store_value<int32_t>(msg, w_[i]);
  }

  int nz, N = 0, S = 0;

  store_value<int32_t>(msg, ndt_.size());
  for (const auto& nt_di : ndt_) {
    std::vector<std::pair<int, int> > sparse_nt_di;
    sparse_nt_di.reserve(K_);
    nz = 0;

    for (int j = 0; j < K_; ++j) {
      if (nt_di[j] > 0) {
        sparse_nt_di.push_back(std::make_pair(j, nt_di[j]));
        nz += 1;
      }
    }

    if (2 * nz < K_) {
      store_value<int8_t>(msg, SPARSE);  // sparse type
      store_value<int16_t>(msg, nz);
      for (auto& a : sparse_nt_di) {
        store_value<int16_t>(msg, a.first);
        store_value<int16_t>(msg, a.second);
      }
      N += nz;
      S += 1;
    } else {
      store_value<int8_t>(msg, DENSE);  // dense type
      int16_t* data = reinterpret_cast<int16_t*>(msg);
      std::copy(nt_di.begin(), nt_di.begin() + K_, data);
      msg = reinterpret_cast<char*>(
          (reinterpret_cast<char*>(msg) + sizeof(int16_t) * K_));
    }
  }

  to_send_size =
      sizeof(int8_t) * ndt_.size() +
      sizeof(int16_t) * (1 + t_.size() + S + 2 * N + (ndt_.size() - S) * K_) +
      sizeof(int32_t) * (2 + 2 * t_.size());

  return msg_begin;
}

char* LDAStatistics::serialize_slice() {
  char* msg = new char[get_serialize_slice_size()];
  char* msg_begin = msg;  // need to keep this pointer to delete later

  store_value<int32_t>(msg, slice_.size());
  for (const auto& v : slice_) {
    store_value<int32_t>(msg, v);
  }

  return msg_begin;
}

int LDAStatistics::get_serialize_size() {
  return (4 + 3 * t_.size() + ndt_.size() * K_ + slice_.size()) *
         sizeof(int32_t);
}

int LDAStatistics::get_serialize_slice_size() {
  return sizeof(int32_t) * (slice_.size() + 1);
}

int LDAStatistics::get_receive_size() {
  return sizeof(uint32_t) * (1 + (slice_.size() + 1) * K_);
}

void LDAStatistics::store_new_stats(LDAModel& model) {
  ndt_.clear();
  t_.clear();

  model.get_ndt(ndt_);
  model.get_t(t_);
}
}  // namespace cirrus
