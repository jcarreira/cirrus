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
#include <deque>

#include <codecfactory.h>

// #include "temp.h"
// #include <boost>

#include <Utils.h>
#include <gamma.h>
#include <LDAModel.h>

// #define MAX_MSG_SIZE (1024 * 1024 * 100)
namespace cirrus {

LDAModel::LDAModel(const char* buffer, const char* info, int to_update) {

  uint32_t receive_size = load_value<uint32_t>(buffer);
  uint32_t original_size = load_value<uint32_t>(buffer);

  // std::cout << "original size: " << original_size << std::endl;
  // std::cout << "receive_size size: " << receive_size << std::endl;

  FastPForLib::IntegerCODEC &codec = *FastPForLib::CODECFactory::getFromName("simdfastpfor256");

  // TODO
  std::vector<uint32_t> to_decompress;
  to_decompress.reserve(receive_size);
  for (int i = 0; i < receive_size; ++i) {
    uint32_t temp = load_value<uint32_t>(buffer);
    to_decompress.push_back(temp);
  }

  // for(int i=0; i<10; ++i){
  //   std::cout << to_decompress[i] << " ";
  // }
  // std::cout << std::endl;

  std::vector<uint32_t> decompressed(original_size);
  size_t recoveredsize = decompressed.size();
  codec.decodeArray(to_decompress.data(), to_decompress.size(), decompressed.data(), recoveredsize);
  decompressed.resize(original_size);

  // for ()

  std::deque<uint32_t> partial_model(decompressed.begin(), decompressed.end());

  update_bucket = to_update;

  // int V_by_K = load_value<int>(buffer);
  // V_ = load_value<int>(buffer);
  V_ = partial_model.front();
  partial_model.pop_front();

  // std::cout << "V: " << V_ << std::endl;

  K_ = load_value<int16_t>(info);
  // K_ = load_value<int>(info);

  // std::cout << V_ << " " << K_ << std::endl;
  // V_ = V_by_K / K_;

  nvt.clear();
  nvt.reserve(V_);
  for (int i = 0; i < V_; ++i) {
    std::vector<int> nt_vi;
    std::vector<int> nz_nt_vi;
    nz_nt_vi.reserve(K_);
    // nt_vi.reserve(K_);
    // int len = load_value<int>(buffer);
    // for (int j = 0; j < len; ++j) {
    //   int top = load_value<int>(buffer);
    //   int count = load_value<int>(buffer);
    //   nt_vi[top] = count;
    // }
    // for (int j = 0; j < K_; ++j) {
    //   int temp = load_value<int>(buffer);
    //   nt_vi.push_back(temp);
    // }

    // uint8_t store_type = load_value<uint8_t>(buffer); // 1 -> sparse, 2 -> dense

    uint32_t store_type = partial_model.front();
    partial_model.pop_front();

    // std::cout << store_type << " ***\n";
    if (store_type == 1) {
      // sparse
      nt_vi.resize(K_, 0);

      // uint16_t len = load_value<uint16_t>(buffer);
      uint32_t len = partial_model.front();
      partial_model.pop_front();

      for (int j = 0; j < len; ++j) {
        // uint16_t top = load_value<uint16_t>(buffer);
        // uint16_t count = load_value<uint16_t>(buffer);

        uint32_t top = partial_model.front();
        partial_model.pop_front();
        uint32_t count = partial_model.front();
        partial_model.pop_front();

        nt_vi[top] = count;
        nz_nt_vi.push_back(top);
      }
    } else if (store_type == 2) {
      // dense
      nt_vi.reserve(K_);

      for (int j = 0; j < K_; ++j) {
        // uint16_t temp = load_value<uint16_t>(buffer);

        uint32_t temp = partial_model.front();
        partial_model.pop_front();

        nt_vi.push_back(temp);
        if (temp != 0) {
          nz_nt_vi.push_back(j);
        }
      }
    } else {
      throw std::runtime_error("Invalid store type");
    }
    nvt.push_back(nt_vi);
    nz_nvt_indices.push_back(nz_nt_vi);
  }

  nt.clear();
  nt.reserve(K_);
  nz_nt_indices.reserve(K_);
  for (int i = 0; i < K_; ++i) {
    // int temp = load_value<int>(buffer);
    uint32_t temp = partial_model.front();
    partial_model.pop_front();
    nt.push_back(temp);
    if (temp != 0) {
      nz_nt_indices.push_back(i);
    }
  }

  t.clear();
  d.clear();
  w.clear();
  int32_t N = load_value<int32_t>(info);
  t.reserve(N);
  d.reserve(N);
  w.reserve(N);
  for (int32_t i = 0; i < N; ++i) {
    int16_t top = load_value<int16_t>(info);
    int16_t doc = load_value<int16_t>(info);
    int32_t word = load_value<int32_t>(info);
    t.push_back(top);
    d.push_back(doc);
    w.push_back(word);
  }

  slice.clear();
  int16_t S = load_value<int16_t>(info);
  slice.reserve(S);
  for (int i = 0; i < S; ++i) {
    int s = load_value<int>(info);
    slice.push_back(s);
  }

  ndt.clear();
  int16_t D = load_value<int16_t>(info);
  ndt.reserve(D);

  std::vector<int> nt_di, nz_nt_di;

  for (int i = 0; i < D; ++i) {
    // nt_di.reserve(K_);
    // nz_nt_di.reserve(K_);

    int8_t store_type = load_value<int8_t>(info);
    nt_di.clear();
    nz_nt_di.clear();

    if (store_type == 1) {
      int16_t len = load_value<int16_t>(info);
      nt_di.resize(K_, 0);
      nz_nt_di.reserve(len);
      for (int j = 0; j < len; ++j) {
        int16_t top = load_value<int16_t>(info);
        int16_t count = load_value<int16_t>(info);
        nt_di[top] = count;
        nz_nt_di.push_back(top);
      }
    } else if (store_type == 2) {
      nt_di.reserve(K_);
      nz_nt_di.reserve(K_);
      for (int j = 0; j < K_; ++j) {
        int16_t temp = load_value<int16_t>(info);
        nt_di.push_back(temp);
        if (temp != 0) {
          nz_nt_di.push_back(j);
        }
      }
    }

    ndt.push_back(nt_di);
    nz_ndt_indices.push_back(nz_nt_di);
  }
}

LDAModel& LDAModel::operator=(LDAModel& model) {
  K_ = model.K_;
  V_ = model.V_;
  update_bucket = model.update_bucket;
  t = std::move(model.t);
  d = std::move(model.d);
  w = std::move(model.w);
  ndt = std::move(model.ndt);
  nvt = std::move(model.nvt);
  nt = std::move(model.nt);
  nz_ndt_indices = std::move(model.nz_ndt_indices);
  nz_nvt_indices = std::move(model.nz_nvt_indices);
  nz_nt_indices = std::move(model.nz_nt_indices);
  slice = std::move(model.slice);
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

  double smoothing_threshold = 0.0, doc_threshold = 0.0;
  std::vector<double> smoothing_vec, doc_vec;

  // Computing [smoothing_threshold]
  // Only iterating over non-zero entries
  smoothing_vec.resize(K_, (alpha * eta) / (eta * V_));
  for (auto& i: nz_nt_indices) {
    double value = (alpha * eta) / (eta * V_ + nt[i]);
    smoothing_threshold += value;
    smoothing_vec[i] = value;
  }
  smoothing_threshold += (alpha * eta) / (eta * V_) * (K_ - nz_nt_indices.size());

  // std::cout << "Smoothing threshold: " << smoothing_threshold << std::endl;

  int pre_doc = -1;

  // coefficients vector to help computing word_threshold
  std::vector<int> coeff_di;

  for (int i = 0; i < t.size(); i++) {
    top = t[i], doc = d[i], gindex = w[i];
    // if (slice_map.find(gindex) == slice_map.end())
    //   continue;
    if (slice_map.at(gindex) == -1)
      continue;
    lindex = slice_map.at(gindex);
    temp++;

    if (doc != pre_doc) {

      pre_doc = doc;

      // Compute doc_threshold for every new document
      doc_threshold = 0.0;
      doc_vec.resize(K_, 0);
      for (auto& nz_top: nz_ndt_indices[doc]) {
        double value = ndt[doc][nz_top] * alpha / (eta * V_ + nt[nz_top]);
        doc_threshold += value;
        doc_vec[nz_top] = value;
      }

      // Compute coefficients vector
      coeff_di.resize(K_, 0);
      for (int j = 0; j < K_; ++j) {
        coeff_di[j] = (alpha + ndt[doc][j]) / (eta * V_ + nt[j]);
        // std::cout << coeff_di[i] << std::endl;
      }

    }

    // if (nvt[lindex][top] <= 0) {
    //   continue;
    // }

    nvt[lindex][top] -= 1;
    ndt[doc][top] -= 1;
    nt[top] -= 1;

    rate_cum = 0.0;
    // std::vector<int> which_topic(K_);
    // which_topic[top] = 1;

    // // Naive Sampler
    // for (int j = 0; j < K_; ++j) {
    //
    //   r = (alpha + ndt[doc][j]) * (eta + nvt[lindex][j]) / (V_ * eta + nt[j]);
    //
    //   if (r > 0)
    //     rate_cum += r;
    //
    //   rate[j] = rate_cum;
    // }
    //
    // linear = rand() * rate_cum / RAND_MAX;
    // new_top = (std::lower_bound(rate, rate + K_, linear)) - rate;

    double word_threshold = 0.0;
    for (auto& nz_top: nz_nvt_indices[lindex]) {
      // std::cout << coeff_di[nz_top] << " " << nvt[lindex][nz_top] << std::endl;
      word_threshold += coeff_di[nz_top] * nvt[lindex][nz_top];
    }

    double rdn = rand() * (smoothing_threshold + doc_threshold + word_threshold) / RAND_MAX;

    // std::cout << doc_threshold << " " << word_threshold << std::endl;

    // 'smoothing only' bucket
    if (rdn < smoothing_threshold) {

      // std::cout << "1\n";

        for (int j = 0; j < K_; ++j) {
          r = smoothing_vec[j] + doc_vec[j] + coeff_di[j] * nvt[lindex][j];
          if (r > rdn) {
            new_top = j;
            break;
          }
          rate[j] = r;
        }

        // Keep adding until we reach a value greater than rdn
        bool finish = true;
        while(finish) {
          for (int j = 0; j < K_; ++j) {
            rate[j] += smoothing_vec[j];
            if (rate[j] > rdn) {
              new_top = j;
              finish = false;
              break;
            }
          }
        }
    } else if (rdn < smoothing_threshold + doc_threshold) {

      // std::cout << "2\n";

      std::vector<double> probs;
      probs.reserve(nz_ndt_indices[doc].size());
      rate_cum = 0.0;

      // O(K_d)
      for (auto& nz_top: nz_ndt_indices[doc]) {
        r = smoothing_vec[nz_top] + doc_vec[nz_top] + coeff_di[nz_top] * nvt[lindex][nz_top];
        if (r > 0) {
          rate_cum += r;
        }
        probs.push_back(rate_cum);
      }
      linear = rand() * rate_cum / RAND_MAX;
      int l_idx = std::lower_bound(probs.begin(), probs.begin() + nz_ndt_indices[doc].size(), linear) - probs.begin();
      new_top = nz_ndt_indices[doc][l_idx];

    } else {

      // std::cout << "3\n";

      std::vector<double> probs;
      probs.reserve(nz_nvt_indices[lindex].size());
      rate_cum = 0.0;

      for (auto& nz_top: nz_nvt_indices[lindex]) {
        r = smoothing_vec[nz_top] + doc_vec[nz_top] + coeff_di[nz_top] * nvt[lindex][nz_top];
        if (r > 0) {
          rate_cum += r;
        }
        probs.push_back(rate_cum);
      }
      linear = rand() * rate_cum / RAND_MAX;
      int l_idx = std::lower_bound(probs.begin(), probs.begin() + nz_nvt_indices[lindex].size(), linear) - probs.begin();
      new_top = nz_nvt_indices[lindex][l_idx];
    }

    t[i] = new_top;
    nvt[lindex][new_top] += 1;
    ndt[doc][new_top] += 1;
    nt[new_top] += 1;

    change_nvt[lindex * K_ + top] -= 1;
    change_nvt[lindex * K_ + new_top] += 1;
    change_nt[top] -= 1;
    change_nt[new_top] += 1;

    // updating doc_threshold, doc_vec & coeff_di
    double to_subtract = doc_vec[top] + doc_vec[new_top];
    doc_vec[top] = ndt[doc][top] * alpha / (eta * V_ + nt[top]);
    doc_vec[new_top] = ndt[doc][new_top] * alpha / (eta * V_ + nt[new_top]);

    doc_threshold += doc_vec[top] + doc_vec[new_top] - to_subtract;

    coeff_di[top] = (alpha + ndt[doc][top]) / (eta * V_ + nt[top]);
    coeff_di[new_top] = (alpha + ndt[doc][new_top]) / (eta * V_ + nt[new_top]);

    // std::cout << "*\n";

  }

  // std::cout << "---\n";

  total_sampled_tokens += temp;

  delete[] rate;

  std::unique_ptr<LDAUpdates> ret = std::make_unique<LDAUpdates>(change_nvt, change_nt, slice, update_bucket);


  // std::unique_ptr<LDAUpdates> ret =
  //     std::make_unique<LDAUpdates>(std::move(change_nvt), std::move(change_nt), slice, update_bucket);
  return ret;
}
}
