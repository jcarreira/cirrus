#define DEBUG

#include <LDAModel.h>
#include <Utils.h>
#include <gamma.h>
#include <math.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <deque>
#include <map>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "lz4.h"

namespace cirrus {
LDAModel::LDAModel(const char* info) {
  // # of potential latent topics
  K_ = load_value<int16_t>(info);

  // load the current topic assignments
  t.clear();
  d.clear();
  w.clear();
  int32_t N = load_value<int32_t>(info);
  t.reserve(N);
  d.reserve(N);
  w.reserve(N);
  for (int32_t i = 0; i < N; ++i) {
    int16_t top = load_value<int16_t>(info);
    int32_t doc = load_value<int32_t>(info);
    int32_t word = load_value<int32_t>(info);
    t.push_back(top);
    d.push_back(doc);
    w.push_back(word);
  }

  // load the doc-topic statistics
  ndt.clear();
  int32_t D = load_value<int32_t>(info);

  nz_ndt_indices_check.clear();
  nz_ndt_indices_check.reserve(D);
  for (int i = 0; i < D; ++i) {
    std::array<int, 1000> temp;
    temp.fill(-1);
    nz_ndt_indices_check.push_back(temp);
  }

  nz_ndt_indices_counter.clear();
  nz_ndt_indices_counter.resize(D, 0);

  ndt.reserve(D);
  nz_ndt_indices.reserve(D);

  std::vector<int> nt_di;
  std::vector<int> nz_nt_di;

  for (int i = 0; i < D; ++i) {
    int8_t store_type = load_value<int8_t>(info);
    nt_di.clear();
    nz_nt_di.clear();

    // if sparse, directly store the nonzero indices
    if (store_type == 1) {
      int16_t len = load_value<int16_t>(info);
      nt_di.resize(K_, 0);
      nz_nt_di.reserve(K_);
      for (int j = 0; j < len; ++j) {
        int16_t top = load_value<int16_t>(info);
        int16_t count = load_value<int16_t>(info);

        nt_di[top] = count;

        nz_nt_di.push_back(top);
        nz_ndt_indices_check[i][top] = nz_ndt_indices_counter[i];
        nz_ndt_indices_counter[i] += 1;
      }
    } else if (store_type == 2) {
      nt_di.reserve(K_);
      nz_nt_di.reserve(K_);
      for (int j = 0; j < K_; ++j) {
        int16_t temp = load_value<int16_t>(info);
        nt_di.push_back(temp);
        // if dense, check if the entry is zero
        if (temp != 0) {
          nz_nt_di.push_back(j);
          nz_ndt_indices_check[i][j] = nz_ndt_indices_counter[i];
          nz_ndt_indices_counter[i] += 1;
        }
      }
    } else {
      std::cout << store_type << " ++\n";
      throw std::runtime_error("Invalid store type");
    }
    ndt.push_back(nt_di);
    nz_ndt_indices.push_back(nz_nt_di);
  }
}

void LDAModel::update_model(const char* buffer_to_decompress,
                            int compressed_size,
                            int uncompressed_size,
                            int slice_id) {
  // decompress the serialized mem that contains the partial
  // global word-topic statistics
  char* buffer_decompressed = new char[uncompressed_size + 1024];
  LZ4_decompress_fast(buffer_to_decompress, buffer_decompressed,
                      uncompressed_size);
  const char* buffer = buffer_decompressed;

  V_ = load_value<int32_t>(buffer);
  local_V = load_value<int32_t>(buffer);

  // load the partial global stat
  nvt.clear();
  nvt.reserve(local_V);
  nz_nvt_indices.clear();
  nz_nvt_indices.reserve(local_V);

  if (nz_nvt_indices_check.size() != local_V) {
    nz_nvt_indices_check.clear();
    nz_nvt_indices_check.reserve(local_V);
    for (int i = 0; i < local_V; ++i) {
      std::array<int, 1000> temp;
      temp.fill(-1);
      nz_nvt_indices_check.push_back(temp);
    }
  } else {
    for (int i = 0; i < local_V; ++i) {
      nz_nvt_indices_check[i].fill(-1);
    }
  }

  nz_nvt_indices_counter.clear();
  nz_nvt_indices_counter.resize(local_V, 0);

  for (int i = 0; i < local_V; ++i) {
    std::vector<int> nt_vi;
    std::vector<int> nz_nt_vi;
    nz_nt_vi.reserve(K_);

    int16_t store_type =
        load_value<int16_t>(buffer);  // 1 -> sparse, 2 -> dense
    if (store_type == 1) {
      // sparse
      nt_vi.resize(K_, 0);

      int16_t len = load_value<int16_t>(buffer);

      for (int j = 0; j < len; ++j) {
        int16_t top = load_value<int16_t>(buffer);
        int16_t count = load_value<int16_t>(buffer);

        nt_vi[top] = count;
        nz_nt_vi.push_back(top);
        nz_nvt_indices_check[i][top] = nz_nvt_indices_counter[i];
        nz_nvt_indices_counter[i] += 1;
      }

    } else if (store_type == 2) {
      // dense
      nt_vi.reserve(K_);

      for (int j = 0; j < K_; ++j) {
        int16_t temp = load_value<int16_t>(buffer);
        nt_vi.push_back(temp);
        if (temp != 0) {
          nz_nt_vi.push_back(j);
          nz_nvt_indices_check[i][j] = nz_nvt_indices_counter[i];
          nz_nvt_indices_counter[i] += 1;
        }
      }
    } else {
      std::cout << store_type << " **\n";
      throw std::runtime_error("Invalid store type");
    }
    nvt.push_back(nt_vi);
    nz_nvt_indices.push_back(nz_nt_vi);
  }

  // load the current topic statistics
  nt.clear();
  nt.reserve(K_);
  nz_nt_indices.clear();
  nz_nt_indices.reserve(K_);
  nz_nt_indices_check.fill(-1);
  for (int i = 0; i < K_; ++i) {
    int32_t temp = load_value<int32_t>(buffer);
    nt.push_back(temp);
    if (temp != 0) {
      nz_nt_indices.push_back(i);
      nz_nt_indices_check[i] = 1;
    }
  }

  // load the local word space
  slice.clear();
  slice.reserve(local_V);
  for (int i = 0; i < local_V; ++i) {
    int32_t s = load_value<int32_t>(buffer);
    slice.push_back(s);
  }

  delete buffer_decompressed;
}

char* LDAModel::serialize_to_S3(uint64_t& to_send_size) {
  uint64_t temp_size = (3 + 3 * t.size() + ndt.size() * K_) * sizeof(int32_t);
  char* msg = new char[temp_size];
  char* msg_begin = msg;

  store_value<int16_t>(msg, K_);

  store_value<int32_t>(msg, t.size());
  for (int i = 0; i < t.size(); ++i) {
    store_value<int16_t>(msg, t[i]);
    store_value<int32_t>(msg, d[i]);
    store_value<int32_t>(msg, w[i]);
  }

  int N = 0, S = 0;
  store_value<int32_t>(msg, ndt.size());
  for (int i = 0; i < ndt.size(); ++i) {
    if (2 * nz_ndt_indices[i].size() < K_) {
      store_value<int8_t>(msg, 1);
      store_value<int16_t>(msg, nz_ndt_indices[i].size());
      for (auto& a : nz_ndt_indices[i]) {
        store_value<int16_t>(msg, a);
        store_value<int16_t>(msg, ndt[i][a]);
      }
      N += nz_ndt_indices[i].size();
      S += 1;
    } else {
      store_value<int8_t>(msg, 2);  // dense type
      int16_t* data = reinterpret_cast<int16_t*>(msg);
      std::copy(ndt[i].begin(), ndt[i].end(), data);
      msg = reinterpret_cast<char*>(
          (reinterpret_cast<char*>(msg) + sizeof(int16_t) * K_));
    }
  }

  to_send_size =
      sizeof(int8_t) * ndt.size() +
      sizeof(int16_t) * (1 + t.size() + S + 2 * N + (ndt.size() - S) * K_) +
      sizeof(int32_t) * (2 + 2 * t.size());

  return msg_begin;
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

// double LDAModel::compute_ll_ndt() {
//   double ll = 0, lgamma_alpha = lda_lgamma(alpha);
//   for (int j = 0; j < ndt.size(); ++j) {
//     int ndj = 0;
//     for (int k = 0; k < K_; ++k) {
//       ndj += ndt[j][k];
//       if (ndt[j][k] > 0) {
//         ll += lda_lgamma(alpha + ndt[j][k]) - lgamma_alpha;
//       }
//     }
//     ll += lda_lgamma(alpha * K_) - lda_lgamma(alpha * K_ + ndj);
//   }
//   return ll;
// }

double LDAModel::compute_ll_ndt() {
  double ll = 0;
  for (int j = 0; j < ndt.size(); ++j) {
    double ll_single_doc = lda_lgamma(K_ * alpha) - K_ * lda_lgamma(alpha);
    int ndj = 0, nz_num = 0;
    for (int k = 0; k < K_; ++k) {
      ndj += ndt[j][k];
      if (ndt[j][k] > 0) {
        ll_single_doc += lda_lgamma(alpha + ndt[j][k]);
        ++nz_num;
      }
    }
    ll_single_doc +=
        (K_ - nz_num) * lda_lgamma(alpha) - lda_lgamma(alpha * K_ + ndj);
    ll += ll_single_doc;
  }
  return ll;
}

char* LDAModel::sample_model(int& total_sampled_tokens,
                             std::vector<int>& slice_indices,
                             uint32_t& to_send_size) {
  return sample_thread(std::ref(t), std::ref(d), std::ref(w), std::ref(nt),
                       std::ref(nvt), std::ref(ndt), std::ref(slice),
                       total_sampled_tokens, slice_indices, to_send_size);
}

char* LDAModel::sample_thread(std::vector<int>& t,
                              std::vector<int>& d,
                              std::vector<int>& w,
                              std::vector<int>& nt,
                              std::vector<std::vector<int> >& nvt,
                              std::vector<std::vector<int> >& ndt,
                              std::vector<int>& slice,
                              int& total_sampled_tokens,
                              std::vector<int>& slice_indices,
                              uint32_t& to_send_size) {
  std::array<int, 1000000> slice_map;
  slice_map.fill(-1);
  int idx = 0;
  for (int i : slice) {
    slice_map.at(i) = idx;
    ++idx;
  }

  total_sampled_tokens = 0;

  // // Naive Sampler
  // double* rate = new double[K_];
  // double r, rate_cum, linear;

  int top, new_top, doc, gindex, lindex, j;
  // std::vector<int>::iterator it;

  // initialize an empty vector to hold the updates
  std::vector<int> change_nvt(nvt.size() * K_, 0);
  std::vector<int> change_nt(K_, 0);
  std::vector<std::set<int> > change_nvt_indices(nvt.size(), std::set<int>());

  double smoothing_threshold = 0.0, doc_threshold = 0.0;
  std::vector<double> smoothing_vec, doc_vec, word_vec, coeff_base;
  std::vector<double> coeff_di;

  // Computing [smoothing_threshold]
  // Only iterating over non-zero entries
  smoothing_vec.resize(K_, (alpha * eta) / (eta * V_));
  for (auto& i : nz_nt_indices) {
    double value = (alpha * eta) / (eta * V_ + nt[i]);
    smoothing_threshold += value;
    smoothing_vec[i] = value;
  }
  smoothing_threshold +=
      (alpha * eta) / (eta * V_) * (K_ - nz_nt_indices.size());

  word_vec.resize(K_, 0);

  // initialize the coefficients which would be needed
  // later to compute word_threshold
  coeff_base.reserve(K_);
  for (int i = 0; i < K_; ++i) {
    double temp = alpha / (eta * (double) V_ + (float) nt[i]);
    coeff_base.push_back(temp);
  }

  int pre_doc = -1;
  for (int i = 0; i < slice_indices.size(); i++) {
    int index = slice_indices[i];
    top = t[index], doc = d[index], gindex = w[index];

    // Check if current word is in the local word space or not
    if (slice_map[gindex] == -1) {
      continue;
    }

    // Get local word idx from global word idx
    lindex = slice_map.at(gindex);

    // True only if model has finished sampling the document
    // with id = pre_doc
    if (doc != pre_doc) {
      // Save the new doc_id
      pre_doc = doc;

      // Compute doc_threshold for every new document
      doc_threshold = 0.0;
      doc_vec.clear();
      doc_vec.resize(K_, 0);
      for (auto& nz_top : nz_ndt_indices[doc]) {
        double value = ndt[doc][nz_top] * eta / (eta * V_ + nt[nz_top]);
        doc_threshold += value;
        doc_vec[nz_top] = value;
      }

      // Compute actual coefficients for later word_threshold
      coeff_di = coeff_base;
      for (auto& nz_top : nz_ndt_indices[doc]) {
        coeff_di[nz_top] = 1.0 * (alpha + (double) ndt[doc][nz_top]) /
                           (eta * (double) V_ + (float) nt[nz_top]);
      }
    }

    // Remove the current token b4 the actual sampling
    nvt[lindex][top] -= 1;
    ndt[doc][top] -= 1;
    nt[top] -= 1;

    // Remove the corresponding non-zero entry's index if needed
    if (ndt[doc][top] == 0 && nz_ndt_indices_check[doc][top] != -1) {
      int ldoc_idx = nz_ndt_indices_check[doc][top];
      for (int dec_doc_idx = ldoc_idx + 1;
           dec_doc_idx < nz_ndt_indices_counter[doc]; dec_doc_idx++) {
        int dec_top = nz_ndt_indices[doc][dec_doc_idx];
        nz_ndt_indices_check[doc][dec_top] -= 1;
      }
      nz_ndt_indices_counter[doc] -= 1;
      nz_ndt_indices_check[doc][top] = -1;
      nz_ndt_indices[doc].erase(nz_ndt_indices[doc].begin() + ldoc_idx);
    }

    if (nvt[lindex][top] == 0 && nz_nvt_indices_check[lindex][top] != -1) {
      int lvocab_idx = nz_nvt_indices_check[lindex][top];
      for (int dec_vocab_idx = lvocab_idx + 1;
           dec_vocab_idx < nz_nvt_indices_counter[lindex]; dec_vocab_idx++) {
        int dec_top = nz_nvt_indices[lindex][dec_vocab_idx];
        nz_nvt_indices_check[lindex][dec_top] -= 1;
      }
      nz_nvt_indices_counter[lindex] -= 1;
      nz_nvt_indices_check[lindex][top] = -1;
      nz_nvt_indices[lindex].erase(nz_nvt_indices[lindex].begin() + lvocab_idx);
    }

    // Update the thresholds due to the current token's removal
    smoothing_threshold -= smoothing_vec[top];
    doc_threshold -= doc_vec[top];

    smoothing_vec[top] = (alpha * eta) / (eta * V_ + nt[top]);
    smoothing_threshold += smoothing_vec[top];

    doc_vec[top] = ndt[doc][top] * eta / (eta * V_ + nt[top]);
    doc_threshold += doc_vec[top];

    coeff_di[top] = 1.0 * (alpha + (double) ndt[doc][top]) /
                    (eta * (double) V_ + (float) nt[top]);
    coeff_base[top] = alpha / (eta * (double) V_ + (float) nt[top]);

    // // Naive Sampler
    // rate_cum = 0.0;
    // std::vector<int> which_topic(K_);
    // which_topic[top] = 1;
    // for (int j = 0; j < K_; ++j) {
    //
    //   // r = (alpha + ndt[doc][j]) * (eta + nvt[lindex][j]) / (V_ * eta +
    // nt[j]);
    //   r = (alpha * eta) / (eta * V_ + nt[j]) +
    //       (ndt[doc][j] * eta) / (eta * V_ + nt[j]) +
    //       ((alpha + ndt[doc][j]) * nvt[lindex][j]) / (eta * V_ + nt[j]);
    //
    //   if (r > 0)
    //     rate_cum += r;
    //
    //   rate[j] = rate_cum;
    // }
    //
    // linear = rand() * rate_cum / RAND_MAX;
    // new_top = (std::lower_bound(rate, rate + K_, linear)) - rate;

    // Compute word_threshold from coeff_di helpers
    double word_threshold = 0.0;
    for (auto& nz_top : nz_nvt_indices[lindex]) {
      word_vec[nz_top] = coeff_di[nz_top] * nvt[lindex][nz_top];
      word_threshold += word_vec[nz_top];
    }

    // Generate a random number which cannot exceed the sum of three thresholds
    double rdn = rand() *
                 (smoothing_threshold + doc_threshold + word_threshold) /
                 RAND_MAX;

    // Currently linear search; O(K_d + K_w)
    // XXX can be improved to log by using F+ tree
    if (rdn < word_threshold) {
      for (auto& nz_top : nz_nvt_indices[lindex]) {
        rdn -= word_vec[nz_top];
        if (rdn <= 0) {
          new_top = nz_top;
          break;
        }
      }
    } else {
      rdn -= word_threshold;
      if (rdn < doc_threshold) {
        for (auto& nz_top : nz_ndt_indices[doc]) {
          rdn -= doc_vec[nz_top];
          if (rdn <= 0) {
            new_top = nz_top;
            break;
          }
        }
      } else {
        rdn -= doc_threshold;
        for (int j = 0; j < K_; ++j) {
          rdn -= smoothing_vec[j];
          if (rdn <= 0) {
            new_top = j;
            break;
          }
        }
      }
    }

    // Add the corresponding non-zero entry's index if needed
    if (ndt[doc][new_top] == 0 && nz_ndt_indices_check[doc][new_top] == -1) {
      nz_ndt_indices[doc].push_back(new_top);
      nz_ndt_indices_check[doc][new_top] = nz_ndt_indices_counter[doc];
      nz_ndt_indices_counter[doc] += 1;
    }

    if (nvt[lindex][new_top] == 0 &&
        nz_nvt_indices_check[lindex][new_top] == -1) {
      nz_nvt_indices[lindex].push_back(new_top);
      nz_nvt_indices_check[lindex][new_top] = nz_nvt_indices_counter[lindex];
      nz_nvt_indices_counter[lindex] += 1;
    }

    if (nt[new_top] == 0 && nz_nt_indices_check[new_top] == -1) {
      nz_nt_indices_check[new_top] = 1;
      nz_nt_indices.push_back(new_top);
    }

    // Apply the update to local statistics
    t[index] = new_top;
    nvt[lindex][new_top] += 1;
    ndt[doc][new_top] += 1;
    nt[new_top] += 1;

    // Add the update
    if (change_nvt[lindex * K_ + top] == 0) {
      change_nvt_indices[lindex].insert(top);
    }

    if (change_nvt[lindex * K_ + new_top] == 0) {
      change_nvt_indices[lindex].insert(new_top);
    }

    change_nvt[lindex * K_ + top] -= 1;
    change_nvt[lindex * K_ + new_top] += 1;
    change_nt[top] -= 1;
    change_nt[new_top] += 1;

    if (change_nvt[lindex * K_ + top] == 0) {
      change_nvt_indices[lindex].erase(top);
    }

    if (change_nvt[lindex * K_ + new_top] == 0) {
      change_nvt_indices[lindex].erase(new_top);
    }

    // Update the thresholds due to the new sampled token
    smoothing_threshold -= smoothing_vec[new_top];
    doc_threshold -= doc_vec[new_top];

    smoothing_vec[new_top] = (alpha * eta) / (eta * V_ + nt[new_top]);
    smoothing_threshold += smoothing_vec[new_top];

    doc_vec[new_top] = ndt[doc][new_top] * eta / (eta * V_ + nt[new_top]);
    doc_threshold += doc_vec[new_top];

    coeff_di[new_top] = 1.0 * (alpha + (double) ndt[doc][new_top]) /
                        (eta * (double) V_ + (float) nt[new_top]);
    coeff_base[new_top] = alpha / (eta * (double) V_ + (float) nt[new_top]);

    total_sampled_tokens += 1;
  }

  // store the number of sampled tokens in this run
  // total_sampled_tokens = temp;

  // // Naive Sampler
  // delete[] rate;

  // The upper bound of the update's size
  uint32_t temp_size = sizeof(uint64_t) +
                       sizeof(int) * (3 + change_nt.size() + 2 * slice.size()) +
                       sizeof(int16_t) * (nvt.size() * change_nt.size() * 2);

  char* mem = new char[temp_size];
  char* mem_begin = mem;

  store_value<uint64_t>(mem, 1);
  store_value<int>(mem, slice.size());
  store_value<uint32_t>(mem, change_nt.size());

  // store the local word space
  int* data = reinterpret_cast<int*>(mem);
  std::copy(slice.begin(), slice.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * slice.size()));

  // store the updates to partial global statistics
  int N = 0;
  for (int i = 0; i < slice.size(); ++i) {
    store_value<int>(mem, change_nvt_indices[i].size());
    for (auto& top : change_nvt_indices[i]) {
      store_value<int16_t>(mem, top);
      store_value<int16_t>(mem, change_nvt[i * K_ + top]);
      N += 1;
    }
  }

  data = reinterpret_cast<int*>(mem);
  std::copy(change_nt.begin(), change_nt.end(), data);
  mem = reinterpret_cast<char*>(
      (reinterpret_cast<char*>(mem) + sizeof(int) * change_nt.size()));

  // XXX remove later
  store_value<uint32_t>(mem, 1);

  // save the actual size
  to_send_size = sizeof(uint64_t) +
                 sizeof(int) * (3 + change_nt.size() + 2 * slice.size()) +
                 sizeof(int16_t) * (2 * N);

  return mem_begin;
}
}  // namespace cirrus
