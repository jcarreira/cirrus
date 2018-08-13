
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

  K_ = load_value<int>(msg);

  int t_size = load_value<int>(msg);
  t_.clear();
  d_.clear();
  w_.clear();
  t_.reserve(t_size);
  d_.reserve(t_size);
  w_.reserve(t_size);
  for (int i = 0; i < t_size; ++i) {
    int t = load_value<int>(msg);
    t_.push_back(t);

    int d = load_value<int>(msg);
    d_.push_back(d);

    int w = load_value<int>(msg);
    w_.push_back(w);
  }

  int s = load_value<int>(msg);
  slice_.clear();
  slice_.reserve(s);

  for (int i = 0; i < s; ++i) {
    int slice_i = load_value<int>(msg);
    slice_.push_back(slice_i);
  }

  int num_docs = load_value<int>(msg);
  ndt_.clear();
  ndt_.reserve(num_docs);


  for (int i = 0; i < num_docs; ++i) {
    std::vector<int> ndt_row;
    ndt_row.reserve(K_);
    for (int j = 0; j < K_; ++j) {
      int ndt_ij = load_value<int>(msg);
      ndt_row.push_back(ndt_ij);
    }
    ndt_.push_back(ndt_row);
  }

  current = 0;
}

char* LDAStatistics::serialize() {

  char* msg = new char[get_serialize_size()];
  char* msg_begin = msg;  // need to keep this pointer to delete later

  store_value<int>(msg, K_);

  store_value<int>(msg, t_.size());
  for (int i = 0; i < t_.size(); ++i) {
    store_value<int>(msg, t_[i]);
    store_value<int>(msg, d_[i]);
    store_value<int>(msg, w_[i]);
  }

  store_value<int>(msg, slice_.size());
  for (const auto& v : slice_) {
    store_value<int>(msg, v);
  }

  store_value<int>(msg, ndt_.size());
  for (const auto& nt_di : ndt_) {
    // store_value<int>(msg, nt_di.size());
    for (const auto& n : nt_di) {
      store_value<int>(msg, n);
    }
  }

  return msg_begin;
}

char* LDAStatistics::serialize_slice() {
  char* msg = new char[get_serialize_slice_size()];
  char* msg_begin = msg;  // need to keep this pointer to delete later

  // issue is here
  store_value<int>(msg, slice_.size());
  for (const auto& v : slice_) {
    store_value<int>(msg, v);
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
  return (1 + slice_.size()) * sizeof(int);
}

int LDAStatistics::get_receive_size() {
  return sizeof(uint32_t) * (1 + (slice_.size() + 1) * K_);
}

int LDAStatistics::pop_partial_slice(
    std::unique_ptr<LDAStatistics>& partial_stat) {
  std::vector<int> slice;
  if (int(slice_.size()) - current < slice_size && current != 0) {
    return -1;
  }

  if (current + slice_size < int(slice_.size()))
    slice = std::vector<int>(slice_.begin() + current,
                             slice_.begin() + current + slice_size);
  else {
    slice = std::vector<int>(slice_.begin() + current, slice_.end());
  }

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

  current += slice_size;
}

}
