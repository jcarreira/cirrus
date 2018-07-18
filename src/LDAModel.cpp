#define DEBUG

#include <algorithm>
#include <chrono>
#include <random>
#include <map>
#include <ctime>
#include <math.h>
#include <set>
#include <map>
// #include <boost>

#include <Utils.h>
#include <gamma.h>
#include <LDAModel.h>

#define MAX_MSG_SIZE (1024*1024*100)

namespace cirrus{


  LDAModel::LDAModel(const char* buffer, const char* info){

    const int* V = reinterpret_cast<const int*>(buffer);
    buffer = reinterpret_cast<const char*>(reinterpret_cast<const char*>(buffer) +  sizeof(int));
    V_ = *V;

    const int* K = reinterpret_cast<const int*>(info);
    K_ = *K;

    info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));

    nvt.clear();
    std::vector<int> nt_vi;
    for(int i=0; i<*V; ++i){
      for(int j=0; j<*K; ++j){
        const int* temp = reinterpret_cast<const int*>(buffer);
        buffer = reinterpret_cast<const char*>(reinterpret_cast<const char*>(buffer) +  sizeof(int));
        nt_vi.push_back(*temp);
      }
      nvt.push_back(nt_vi);
      nt_vi.clear();
    }


    nt.clear();
    for(int i=0; i<*K; ++i){
      const int* temp = reinterpret_cast<const int*>(buffer);
      buffer = reinterpret_cast<const char*>(reinterpret_cast<const char*>(buffer) +  sizeof(int));
      nt.push_back(*temp);
    }


    t.clear();
    d.clear();
    w.clear();
    const int* N = reinterpret_cast<const int*>(info);
    info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
    for(int i=0; i<*N; ++i){
      const int* top = reinterpret_cast<const int*>(info);
      info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
      const int* doc = reinterpret_cast<const int*>(info);
      info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
      const int* word = reinterpret_cast<const int*>(info);
      info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
      t.push_back(*top);
      d.push_back(*doc);
      w.push_back(*word);
    }


    ndt.clear();
    std::vector<int> nt_di;
    const int* D = reinterpret_cast<const int*>(info);
    info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
    for(int i=0; i<*D; ++i){
      for(int j=0; j<*K; ++j){
        const int* temp = reinterpret_cast<const int*>(info);
        info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
        nt_di.push_back(*temp);
      }
      ndt.push_back(nt_di);
      nt_di.clear();
    }


    slice.clear();
    const int* S = reinterpret_cast<const int*>(info);
    info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
    for(int i=0; i<*S; ++i){
      const int* s = reinterpret_cast<const int*>(info);
      info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
      slice.push_back(*s);
    }
  }

  std::unique_ptr<LDAUpdates> LDAModel::sample_model(){

    return sample_thread(std::ref(t), std::ref(d),
                      std::ref(w), std::ref(nt), std::ref(nvt), // std::ref(nvt),
                      std::ref(ndt), std::ref(slice));
  }

  std::unique_ptr<LDAUpdates> LDAModel::sample_thread(std::vector<int>& t,
                              std::vector<int>& d,
                              std::vector<int>& w,
                              std::vector<int>& nt,
                              std::vector<std::vector<int>>& nvt,
                              std::vector<std::vector<int>>& ndt,
                              std::vector<int>& slice
                            ){

    std::map<int, int> slice_map;
    int idx = 0;
    for(int i: slice){
      slice_map.insert(std::make_pair(i, idx));
      ++ idx;
    }

    double* rate = new double[K_];
    double r, rate_cum, linear;
    int top, new_top, doc, gindex, lindex, j;//, lindex, j;
    std::vector<int>::iterator it;

    std::vector<int> change_nvt(nvt.size() * K_);
    std::vector<int> change_nt(K_);

    for(int i=0; i<t.size(); i++){

      top = t[i], doc = d[i], gindex = w[i] - 1;

      if(slice_map.find(gindex) == slice_map.end())
        continue;
      lindex = slice_map[gindex];

      nvt[lindex][top] -= 1;
      ndt[doc][top] -= 1;
      nt[top] -= 1;

      rate_cum = 0.0;
      for(int j=0; j<K_; ++j){
        r = (alpha + ndt[doc][j]) * (eta + nvt[lindex][j]) / (V_ * eta + nt[j]);
        if(r>0)
          rate_cum += r;

        rate[j] = rate_cum;
      }

      linear = rand()*rate_cum/RAND_MAX;
      new_top = (std::lower_bound(rate, rate+K_, linear)) - rate;

      t[i] = new_top;
      nvt[lindex][new_top] += 1;
      ndt[doc][new_top] += 1;
      nt[new_top] += 1;

      change_nvt[lindex * K_ + top] -= 1;
      change_nvt[lindex * K_ + new_top] += 1;
      change_nt[top] -= 1;
      change_nt[new_top] += 1;

    }

    delete[] rate;

    std::unique_ptr<LDAUpdates> ret = std::make_unique<LDAUpdates>(change_nvt, change_nt, slice);
    return ret;
  }


  // double LDAModel::loglikelihood(){
  //
  //   double lgamma_eta = lda_lgamma(eta), lgamma_alpha = lda_lgamma(alpha);
  //   double ll = 0;
  //   ll += K_ * lda_lgamma(eta * V_);
  //   for(int i=0; i<nworkers_; i++){
  //     for(int j=0; j<ndts[i].size(); j++){
  //       int ndj = 0;
  //       for(int k=0; k<K_; k++){
  //         ndj += ndts[i][j][k];
  //         if(ndts[i][j][k] > 0)
  //           ll += lda_lgamma(alpha + ndts[i][j][k]) - lgamma_alpha;
  //       }
  //       ll += lda_lgamma(alpha * K_) - lda_lgamma(alpha * K_ + ndj);
  //     }
  //   }
  //   for(int i=0; i<K_; i++){
  //     ll -= lda_lgamma(eta * V_ + global_nt[i]);
  //     for(int v=0; v<V_; ++v){
  //       if(global_nvt[v][i] > 0)
  //         ll += lda_lgamma(eta + global_nvt[v][i]) - lgamma_eta;
  //     }
  //   }
  //   return ll;
  // }


  // char* LDAModel::get_partial_model(const char* slice){
  //
  //   std::vector<std::vector<int>> partial_nvt;
  //
  //   const int* len = reinterpret_cast<const int*>(slice);
  //   slice = reinterpret_cast<const char*>(reinterpret_cast<const char*>(slice) +  sizeof(int));
  //   for(int i=0; i<*len; ++i){
  //     const int* word_idx = reinterpret_cast<const int*>(slice);
  //     partial_nvt.push_back(global_nvt[*word_idx]);
  //     slice = reinterpret_cast<const char*>(reinterpret_cast<const char*>(slice) +  sizeof(int));
  //   }
  //
  //   char* mem = new char[MAX_MSG_SIZE];
  //   char* mem_begin = mem;
  //   store_value<int>(mem, total vocab dim) //TODO
  //   store_value<int>(mem, partial_nvt.size());
  //   for(const auto& nt_vi: partial_nvt){
  //     for(const auto& n: nt_vi){
  //       store_value<int>(mem, n);
  //     }
  //   }
  //
  //   for(int i=0; i<K_; ++i){
  //     store_value<int>(mem, global_nt[i]);
  //   }
  //
  //   return mem_begin;
  // }

  // void LDAModel::update_globals(const LDAUpdates* update){
  //
  //   for(int i=0; i<update->slice.size(); ++i){
  //     for(int j=0; j<K_; ++j){
  //       global_nvt[update->slice[i]][j] += update->change_nvt[i*K_ + j];
  //     }
  //   }
  //
  //   for(int i=0; i<K_; ++i){
  //     global_nt[i] += update->change_nt[i];
  //   }
  // }

}
