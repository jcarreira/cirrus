#include <unistd.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include <InputReader.h>
#include <Configuration.h>
#include "gamma.h"
#include "LDAStatistics.h"
#include "ModelGradient.h"

using namespace cirrus;

cirrus::Configuration config = cirrus::Configuration("../../configs/lda_test.cfg");
std::mutex model_lock;
std::vector<int> nvt, nt;
std::vector<LDAStatistics> local_vars;
LDAUpdates global_var;

LDADataset read_dataset(
    const Configuration& config) {

  InputReader input;
  return input.read_lda_input(
      config.get_doc_path(),
      config.get_vocab_path(),
      ",",
      config);
}

LDAStatistics count_dataset(
                    const std::vector<std::vector<std::pair<int, int>>>& docs,\
                    std::vector<int>& nvt,
                    std::vector<int>& nt, int K,
                    std::set<int>& global_vocab){

  std::vector<int> t, d, w;
  std::vector<std::vector<int>> ndt;
  std::set<int> local_vocab;

  for(const auto& doc: docs){

    std::vector<int> ndt_row(K);

    for(const auto& feat: doc){
      int gindex = feat.first, count = feat.second;
      // gindex -= 1;
      if(local_vocab.find(gindex) == local_vocab.end())
        local_vocab.insert(local_vocab.begin(), gindex);
      if(global_vocab.find(gindex) == global_vocab.end())
        global_vocab.insert(global_vocab.begin(), gindex);
      for(int i=0; i<count; ++i){
        int top = rand() % K;

        t.push_back(top);
        d.push_back(ndt.size());
        w.push_back(gindex);
        ++ ndt_row[top];

        nvt[gindex * K + top] += 1;
        nt[top] += 1;
      }
    }
    ndt.push_back(ndt_row);
  }
  std::vector<int> local_vocab_vec(local_vocab.begin(), local_vocab.end());

  return LDAStatistics(K, ndt, local_vocab_vec, t, d, w);
}

double compute_ll(LDAUpdates& global_var, std::vector<LDAStatistics>& local_vars){
  std::vector<int> nvt, nt;
  global_var.get_nvt(nvt);
  global_var.get_nt(nt);

  double alpha = 0.1, eta = .01;
  int K = nt.size();
  int V = nvt.size() / K;
  double lgamma_eta = lda_lgamma(eta), lgamma_alpha = lda_lgamma(alpha);
  double ll = K * lda_lgamma(eta * V);

  for(int i=0; i<K; ++i){
    int nti = 0;
    ll -= lda_lgamma(eta * V + nt[i]);
    for(int v=0; v<V; ++v){
      if(nvt[v * K + i] > 0){
        ll += lda_lgamma(eta + nvt[v * K + i]) - lgamma_eta;
        nti += nvt[v*K + i];
      }
    }
  }

  for(int i=0; i<local_vars.size(); ++i){
    std::vector<std::vector<int>> ndt;
    std::vector<int> slice;
    local_vars[i].get_ndt(ndt);
    local_vars[i].get_slice(slice);
    for(int j=0; j<ndt.size(); ++j){
      int ndj = 0;
      for(int k=0; k<K; ++k){
        ndj += ndt[j][k];
        if(ndt[j][k] > 0){
          ll += lda_lgamma(alpha + ndt[j][k]) - lgamma_alpha;
        }
      }
      ll += lda_lgamma(alpha * K) - lda_lgamma(alpha * K + ndj);
    }
  }
  return ll;
}

void sample(int i){

  std::vector<int> nvt_;

  local_vars[i].set_slice_size(config.get_slice_size());
  std::unique_ptr<LDAStatistics> local_var_slice;
  while(1){
    if(local_vars[i].pop_partial_slice(local_var_slice) == -1){
      local_vars[i].reset_current();
      continue;
    }
    else{
      uint32_t to_send_size;

      model_lock.lock();

      char* partial_model = global_var.get_partial_model(local_var_slice->serialize_slice(), to_send_size);
      LDAModel model(partial_model, local_var_slice->serialize());
      auto update = model.sample_model();

      local_vars[i].store_new_stats(model);
      global_var.update(*update.get());

      model_lock.unlock();
    }
  }
}

int main() {

  uint64_t num_threads = 10;

  // Reading files & compute initial stats
  LDADataset dataset = read_dataset(config);
  int K = config.get_k();

  nvt.resize(dataset.num_vocabs() * K);
  nt.resize(K);
  std::set<int> global_vocab;

  int sample_size = dataset.num_docs() / num_threads;
  dataset.set_sample_size(sample_size);
  std::vector<std::vector<std::pair<int, int>>> partial_docs;
  for(int i=0; i<num_threads; ++i){
    dataset.get_some_docs(partial_docs);
    LDAStatistics local_var = count_dataset(partial_docs, nvt, nt, K, global_vocab);
    local_vars.push_back(local_var);
  }
  std::vector<int> global_vocab_vec(global_vocab.begin(), global_vocab.end());

  global_var = LDAUpdates(nvt, nt, global_vocab_vec);

  // Sampling
  std::vector<std::shared_ptr<std::thread>> threads;
  for (uint64_t i = 0; i < num_threads; ++i) {
    threads.push_back(
        std::make_shared<std::thread>(sample, i));
  }

  // Eval

  std::cout << "Trainning for 30 sec ...\n";

  usleep(30000000); // 10 s

  for (uint64_t i = 0; i < num_threads; ++i) {
    (*threads[i]).detach();
  }

  model_lock.lock();
  double ll = compute_ll(global_var, local_vars);
  model_lock.unlock();
  std::cout << "Loglikelihood after 30 sec trainning: " << ll << std::endl;
  if(ll >= -9.95e6){
    std::cout << "LDA test successed.\n";
  }
  else
    throw std::runtime_error("LDA test failed.");


}
