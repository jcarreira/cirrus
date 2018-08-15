#define DEBUG

#include <algorithm>
#include <chrono>
#include <random>
#include <unordered_map>
#include <array>
#include <ctime>
#include <math.h>
#include <set>
#include <map>
// #include <boost>

#include <Utils.h>
#include <gamma.h>
#include <LDAModel.h>

// #define MAX_MSG_SIZE (1024 * 1024 * 100)

namespace cirrus {

LDAModel::LDAModel(const char* buffer, const char* info, int to_update) {

  update_bucket = to_update;

  // int V_by_K = load_value<int>(buffer);
  V_ = load_value<int>(buffer);

  K_ = load_value<int>(info);
  // V_ = V_by_K / K_;

  nvt.clear();
  nvt.reserve(V_);
  for (int i = 0; i < V_; ++i) {
    std::vector<int> nt_vi(K_, 0);
    // nt_vi.reserve(K_);
    int len = load_value<int>(buffer);
    for (int j = 0; j < len; ++j) {
      int top = load_value<int>(buffer);
      int count = load_value<int>(buffer);
      nt_vi[top] = count;
    }
    // for (int j = 0; j < K_; ++j) {
    //   int temp = load_value<int>(buffer);
    //   nt_vi.push_back(temp);
    // }
    nvt.push_back(nt_vi);
  }

  nt.clear();
  nt.reserve(K_);
  for (int i = 0; i < K_; ++i) {
    int temp = load_value<int>(buffer);
    nt.push_back(temp);
  }

  t.clear();
  d.clear();
  w.clear();
  int N = load_value<int>(info);
  t.reserve(N);
  d.reserve(N);
  w.reserve(N);
  for (int i = 0; i < N; ++i) {
    int top = load_value<int>(info);
    int doc = load_value<int>(info);
    int word = load_value<int>(info);
    t.push_back(top);
    d.push_back(doc);
    w.push_back(word);
  }

  slice.clear();
  int S = load_value<int>(info);
  slice.reserve(S);
  for (int i = 0; i < S; ++i) {
    int s = load_value<int>(info);
    slice.push_back(s);
  }

  ndt.clear();
  int D = load_value<int>(info);
  ndt.reserve(D);
  for (int i = 0; i < D; ++i) {
    std::vector<int> nt_di;
    nt_di.reserve(K_);
    for (int j = 0; j < K_; ++j) {
      int temp = load_value<int>(info);
      nt_di.push_back(temp);
    }
    ndt.push_back(nt_di);
  }
}

std::unique_ptr<LDAUpdates> LDAModel::sample_model(int& total_sampled_tokens) {
  return sample_thread(std::ref(t), std::ref(d), std::ref(w), std::ref(nt),
                       std::ref(nvt), std::ref(ndt), std::ref(slice), total_sampled_tokens);
}

std::unique_ptr<LDAUpdates> LDAModel::sample_thread(
    std::vector<int>& t,
    std::vector<int>& d,
    std::vector<int>& w,
    std::vector<int>& nt,
    std::vector<std::vector<int>>& nvt,
    std::vector<std::vector<int>>& ndt,
    std::vector<int>& slice,
    int& total_sampled_tokens) {

  std::array<int , 1000000> slice_map;
  slice_map.fill(-1);

  // std::unordered_map<int, int> slice_map;
  int idx = 0;
  for (int i : slice) {
    // slice_map.insert(std::make_pair(i, idx));
    // ++idx;
    slice_map.at(i) = idx;
    ++idx;
  }

  double* rate = new double[K_];
  double r, rate_cum, linear;
  int top, new_top, doc, gindex, lindex, j;
  std::vector<int>::iterator it;

  std::vector<int> change_nvt(nvt.size() * K_);
  std::vector<int> change_nt(K_);

  int temp = 0;

  for (int i = 0; i < t.size(); i++) {
    top = t[i], doc = d[i], gindex = w[i];
    // if (slice_map.find(gindex) == slice_map.end())
    //   continue;
    if (slice_map.at(gindex) == -1)
      continue;
    lindex = slice_map.at(gindex);
    temp++;

    nvt[lindex][top] -= 1;
    ndt[doc][top] -= 1;
    nt[top] -= 1;

    rate_cum = 0.0;
    // std::vector<int> which_topic(K_);
    // which_topic[top] = 1;

    for (int j = 0; j < K_; ++j) {

      // Naive Sampler
      r = (alpha + ndt[doc][j]) * (eta + nvt[lindex][j]) / (V_ * eta + nt[j]);

      if (r > 0)
        rate_cum += r;

      rate[j] = rate_cum;
    }

    linear = rand() * rate_cum / RAND_MAX;
    new_top = (std::lower_bound(rate, rate + K_, linear)) - rate;

    t[i] = new_top;
    nvt[lindex][new_top] += 1;
    ndt[doc][new_top] += 1;
    nt[new_top] += 1;

    change_nvt[lindex * K_ + top] -= 1;
    change_nvt[lindex * K_ + new_top] += 1;
    change_nt[top] -= 1;
    change_nt[new_top] += 1;

  }

  total_sampled_tokens += temp;

  delete[] rate;

  std::unique_ptr<LDAUpdates> ret = std::make_unique<LDAUpdates>(change_nvt, change_nt, slice, update_bucket);


  // std::unique_ptr<LDAUpdates> ret =
  //     std::make_unique<LDAUpdates>(std::move(change_nvt), std::move(change_nt), slice, update_bucket);
  return ret;
}
}
