#include <iostream>
#include <string.h>
#include <Utils.h>
#include <LDAStatistics.h>

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

    int16_t d = load_value<int16_t>(msg);
    d_.push_back(d);

    int32_t w = load_value<int32_t>(msg);
    w_.push_back(w);
  }

  int32_t s = load_value<int32_t>(msg);
  slice_.clear();
  slice_.reserve(s);

  for (int i = 0; i < s; ++i) {
    int slice_i = load_value<int>(msg);
    slice_.push_back(slice_i);
  }

  int16_t num_docs = load_value<int16_t>(msg);
  ndt_.clear();
  ndt_.reserve(num_docs);
  std::vector<int> ndt_row;

  for (int i = 0; i < num_docs; ++i) {

    int8_t store_type = load_value<int8_t>(msg);
    ndt_row.clear();

    if (store_type == 1) {
      ndt_row.resize(K_, 0);
      int16_t len = load_value<int16_t>(msg);
      for (int j = 0; j < len; ++j) {
        int16_t top = load_value<int16_t>(msg);
        int16_t count = load_value<int16_t>(msg);
        ndt_row[top] = count;
      }
    } else if (store_type == 2) {
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
    store_value<int16_t>(msg, d_[i]);
    store_value<int32_t>(msg, w_[i]);
  }

  store_value<int32_t>(msg, slice_.size());
  for (const auto& v : slice_) {
    store_value<int32_t>(msg, v);
  }

  int nz, N = 0, S = 0, sparse_type = 1, dense_type = 2;

  store_value<int16_t>(msg, ndt_.size());
  for (const auto& nt_di : ndt_) {

    // sparse_nt_di.clear();
    std::vector<std::pair<int, int>> sparse_nt_di;
    sparse_nt_di.reserve(K_);
    nz = 0;

    for (int j = 0; j < K_; ++j) {
      if (nt_di[j] > 0) {
        sparse_nt_di.push_back(std::make_pair(j, nt_di[j]));
        nz += 1;
      }
    }

    if (2 * nz < K_) {
      store_value<int8_t>(msg, sparse_type); // sparse type
      store_value<int16_t>(msg, nz);
      for (auto& a: sparse_nt_di) {
        store_value<int16_t>(msg, a.first);
        store_value<int16_t>(msg, a.second);
      }
      N += nz;
      S += 1;
    } else {
      store_value<int8_t>(msg, dense_type); // dense type
      int16_t* data = reinterpret_cast<int16_t*>(msg);
      std::copy(nt_di.begin(), nt_di.begin() + K_, data);
      msg = reinterpret_cast<char*>((reinterpret_cast<char*>(msg) + sizeof(int16_t) * K_));
    }
  }

  to_send_size = sizeof(int8_t) * ndt_.size() +
                 sizeof(int16_t) * (2 + 2 * t_.size() + S + 2 * N + (ndt_.size() - S) * K_) +
                 sizeof(int32_t) * (2 + t_.size() + slice_.size());

  return msg_begin;

}

char* LDAStatistics::serialize_slice() {
  char* msg = new char[get_serialize_slice_size()];
  char* msg_begin = msg;  // need to keep this pointer to delete later

  // issue is here
  store_value<int32_t>(msg, slice_.size());
  for (const auto& v : slice_) {
    store_value<int32_t>(msg, v);
  }
  // std::copy(slice_.begin(), slice_.end(), msg);

  return msg_begin;
}

// 1. store the total number of words and the current topic assignments
// 2. store the size of corpus and word counts for each (doc_id, topic) entries
// 3. store the length of local slice and word_idx for each vocabularies
int LDAStatistics::get_serialize_size() {
  return (4 + 3 * t_.size() + ndt_.size() * K_ + slice_.size()) * sizeof(int);
}

int LDAStatistics::get_serialize_slice_size() {
  return sizeof(int32_t) * (slice_.size() + 1);
}

int LDAStatistics::get_receive_size() {
  // std::cout << "computing receive size " << slice_.size() << " " << K_ << std::endl;
  return sizeof(uint32_t) * (1 + (slice_.size() + 1) * K_);
}

int LDAStatistics::pop_partial_slice(
    std::unique_ptr<LDAStatistics>& partial_stat) {
  std::vector<int> slice;

  if (current >= slice_.size() && current != 0) {
    return -1;
  }

  if (current + slice_size < int(slice_.size()))
    slice = std::vector<int>(slice_.begin() + current,
                             slice_.begin() + current + slice_size);
  else {
    slice = std::vector<int>(slice_.begin() + current, slice_.end());
  }

  // incre_current();

  partial_stat.reset(new LDAStatistics(K_, ndt_, slice, t_, d_, w_));
  return 1;
}

void LDAStatistics::store_new_stats(LDAModel& model) {
// void LDAStatistics::store_new_stats() {

  ndt_.clear();
  t_.clear();

  // TODO: use smart pointer / std::move
  model.get_ndt(ndt_);
  model.get_t(t_);

  incre_current();
}

}
