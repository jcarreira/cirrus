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

    const int* V_by_K = reinterpret_cast<const int*>(buffer);
    buffer = reinterpret_cast<const char*>(reinterpret_cast<const char*>(buffer) +  sizeof(int));

    const int* K = reinterpret_cast<const int*>(info);
    K_ = *K;
    V_ = *V_by_K / K_;

    info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));

    nvt.clear();
    std::vector<int> nt_vi;
    for(int i=0; i<V_; ++i){
      for(int j=0; j<K_; ++j){
        const int* temp = reinterpret_cast<const int*>(buffer);
        buffer = reinterpret_cast<const char*>(reinterpret_cast<const char*>(buffer) +  sizeof(int));
        nt_vi.push_back(*temp);
      }
      nvt.push_back(nt_vi);
      nt_vi.clear();
    }


    nt.clear();
    for(int i=0; i<K_; ++i){
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

    slice.clear();
    const int* S = reinterpret_cast<const int*>(info);
    info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
    for(int i=0; i<*S; ++i){
      const int* s = reinterpret_cast<const int*>(info);
      info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
      slice.push_back(*s);
    }

    ndt.clear();
    std::vector<int> nt_di;
    const int* D = reinterpret_cast<const int*>(info);
    info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
    for(int i=0; i<*D; ++i){
      for(int j=0; j<K_; ++j){
        const int* temp = reinterpret_cast<const int*>(info);
        info = reinterpret_cast<const char*>(reinterpret_cast<const char*>(info) +  sizeof(int));
        nt_di.push_back(*temp);
      }
      ndt.push_back(nt_di);
      nt_di.clear();
    }

    // std::cout << "nvt size: " << nvt.size() << " ----------- " << std::endl;
    // std::cout << "K size: " << nvt[0].size() << " ----------- " << std::endl;
    // std::cout << "ndt size: " << ndt.size() << " ----------- " << std::endl;
    // std::cout << "K (ndt) size: " << ndt[0].size() << " ----------- " << std::endl;
  }

  std::unique_ptr<LDAUpdates> LDAModel::sample_model(){

    // std::cout << "b4 sampling ndt[0]: ";
    // for(int i=0; i<K_; ++i){
    //   std::cout << ndt[0][i] << " ";
    // }
    // std::cout << std::endl;

    return sample_thread(std::ref(t), std::ref(d),
                      std::ref(w), std::ref(nt), std::ref(nvt), // std::ref(nvt),
                      std::ref(ndt), std::ref(slice));

    // std::cout << "adter sampling ndt[0]: ";
    // for(int i=0; i<K_; ++i){
    //   std::cout << ndt[0][i] << " ";
    // }
    // std::cout << std::endl;

    // return rst;
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

    // for(int i=0; i<10; ++i){
    //   std::cout << slice[i] << " ";
    // }
    // std::cout << std::endl;
    // for(int i=0; i<10; ++i){
    //   std::cout << slice_map[slice[i]] << " ";
    // }
    // std::cout << std::endl;

    double* rate = new double[K_];
    double r, rate_cum, linear;
    int top, new_top, doc, gindex, lindex, j;//, lindex, j;
    std::vector<int>::iterator it;

    std::vector<int> change_nvt(nvt.size() * K_);
    std::vector<int> change_nt(K_);

    // std::cout << "b4 t: ";
    // for(int i=0; i<10; ++i){
    //   std::cout << t[i] << " ";
    // }
    // std::cout << std::endl;

    // if(slice[0] == 1004){
    //   std::cout << "nvt[0] b4: ";
    //   for(int i=0; i<K_; ++i){
    //     std::cout << nvt[0][i] << " ";
    //   }
    //   std::cout << std::endl;
    // }

    // std::cout << "t.size(): " << t.size() << std::endl;

    // for(int i=0; i<nvt[nvt.size()-1].size(); ++i){
    //   std::cout << nvt[0][i] << " ";
    // }
    // std::cout << std::endl;

    for(int i=0; i<t.size(); i++){

      // std::cout << i << std::endl;

      top = t[i], doc = d[i], gindex = w[i];
      // if(std::find(slice.begin(), slice.end(), gindex) == slice.end())
      //   continue;
      if(slice_map.find(gindex) == slice_map.end())
        continue;
      lindex = slice_map[gindex];

      // std::cout << i << " | " <<  "topic: " << top << " doc: " << doc << " gindex: " << gindex << " lindex: " << lindex << " | " << " V: " << nvt.size() << " D: " << ndt.size() << std::endl;

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

      // if(i < 10){
      //   std::cout << i << " " << top << " " << new_top << std::endl;
      // }

      t[i] = new_top;
      nvt[lindex][new_top] += 1;
      ndt[doc][new_top] += 1;
      nt[new_top] += 1;

      change_nvt[lindex * K_ + top] -= 1;
      change_nvt[lindex * K_ + new_top] += 1;
      change_nt[top] -= 1;
      change_nt[new_top] += 1;

    }

    // if(slice[0] == 1004){
    //   std::cout << "update[0] b4: ";
    //   for(int i=0; i<K_; ++i){
    //     std::cout << change_nvt[i] << " ";
    //   }
    //   std::cout << std::endl;
    //
    //   std::cout << "nvt[0] after: ";
    //   for(int i=0; i<K_; ++i){
    //     std::cout << nvt[0][i] << " ";
    //   }
    //   std::cout << std::endl;
    // }

    delete[] rate;

    // std::cout << "after t: ";
    // for(int i=0; i<10; ++i){
    //   std::cout << t[i] << " ";
    // }
    // std::cout << std::endl;

    // for(int i=0; i<change_nt.size(); ++i){
    //   std::cout << change_nt[i] << " ";
    // }
    // std::cout << std::endl;
    //
    // std::cout << "change_nvt size: " << change_nvt.size() << " ----------- " << std::endl;
    // std::cout << "change_nt size: " << change_nt.size() << " ----------- " << std::endl;
    // std::cout << "slice size: " << slice.size() << " ----------- " << std::endl;

    std::unique_ptr<LDAUpdates> ret = std::make_unique<LDAUpdates>(change_nvt, change_nt, slice);
    return ret;
  }

}
