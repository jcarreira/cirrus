
#include <iostream>
#include <string.h>
#include <Utils.h>
#include <LDAStatistics.h>

#define MAX_MSG_SIZE (1024*1024*100)

namespace cirrus{
  LDAStatistics::LDAStatistics(){
  }

  LDAStatistics::LDAStatistics(int K, std::vector<std::vector<int> >& ndt, std::vector<int>& slice, std::vector<int>& t, std::vector<int>& d, std::vector<int>& w){
    K_ = K;
    ndt_ = ndt;
    slice_ = slice;
    t_ = t;
    d_ = d;
    w_ = w;
  }

  LDAStatistics::LDAStatistics(const char* msg){

    const int* K = reinterpret_cast<const int*>(msg);
    msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
    K_ = *K;

    const int* t_size = reinterpret_cast<const int*>(msg);
    msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
    for(int i=0; i<*t_size; ++i){

      const int* t = reinterpret_cast<const int*>(msg);
      msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
      t_.push_back(*t);

      const int* d = reinterpret_cast<const int*>(msg);
      msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
      d_.push_back(*d);

      const int* w = reinterpret_cast<const int*>(msg);
      msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
      w_.push_back(*w);
    }

    const int* num_docs = reinterpret_cast<const int*>(msg);
    msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
    for(int i=0; i<*num_docs; ++i){

      const int* num_words = reinterpret_cast<const int*>(msg);
      msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));

      std::vector<int> ndt_row(*num_words);
      for(int j=0; j<*num_words; ++j){
        const int* ndt_ij = reinterpret_cast<const int*>(msg);
        msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
        ndt_row.push_back(*ndt_ij);
      }
      ndt_.push_back(ndt_row);
    }

    const int* slice_size = reinterpret_cast<const int*>(msg);
    msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
    for(int i=0; i<*slice_size; ++i){

      const int* slice_i = reinterpret_cast<const int*>(msg);
      msg = reinterpret_cast<const char*>(reinterpret_cast<const char*>(msg) +  sizeof(int));
      slice_.push_back(*slice_i);

    }
  }

  char* LDAStatistics::serialize(){

    char* msg = new char[MAX_MSG_SIZE];
    char* msg_begin = msg; // need to keep this pointer to delete later

    store_value<int>(msg, K_);

    store_value<int>(msg, t_.size());
    for(int i=0; i<t_.size(); ++i){
      store_value<int>(msg, t_[i]);
      store_value<int>(msg, d_[i]);
      store_value<int>(msg, w_[i]);
    }

    // ndt should be sparse
    // TODO: can be improved
    store_value<int>(msg, ndt_.size());
    for(const auto& nt_di: ndt_){
      store_value<int>(msg, nt_di.size());
      for(const auto& n: nt_di){
        store_value<int>(msg, n);
      }
    }

    store_value<int>(msg, slice_.size());
    for(const auto& v: slice_){
      store_value<int>(msg, v);
    }

    return msg_begin;
  }

  char* LDAStatistics::serialize_slice(){

    char* msg = new char[MAX_MSG_SIZE];
    char* msg_begin = msg; // need to keep this pointer to delete later

    store_value<int>(msg, slice_.size());
    for(const auto& v: slice_){
      store_value<int>(msg, v);
    }

    return msg_begin;
  }

  int LDAStatistics::get_serialize_size(){
    return (3 + ndt_.size() * (1 + K_) + slice_.size()) * sizeof(int);
  }

  int LDAStatistics::get_serialize_slice_size(){
    return (1 + slice_.size()) * sizeof(int);
  }

  int LDAStatistics::get_receive_size(){
    return (2 + (slice_.size() + 1) * K_) * sizeof(int);
  }

  char* LDAStatistics::pop_partial_docs(int minibatch_size){
    std::vector<std::vector<int> > ndt_partial;
    std::vector<int> t_partial, d_partial, w_partial;
    std::set<int> slice_partial;

    int start = d_.front(), cur = d_.front();
    while(cur - start < minibatch_size){
      if(get_num_docs() == 0)
        break;

      if(d_.front() != cur ){
        cur = d_.front();
        ndt_partial.push_back(ndt_.front());
        ndt.erase(ndt_.begin());
      }else{
        int gindex = w_.front();
        t_partial.push_back(t_.front());
        d_partial.push_back(d_.front());
        w_partial.push_back(w_.front());
        t_.erase(t_.begin());
        d_.erase(d_.begin());
        w_.erase(w_.begin());
        if(slice_partial.find(gindex) == slice_partial.end())
          slice_partial.push_back(gindex);
      }
    }

    std::vector<int> slice_partial_vec(slice_partial.begin(), slice_partial.end());
    LDAStatistics partial(K_, ndt_partial, slice_partial_vec, t_partial, d_partial, w_partial);
    return partial.serialize();
  }

  int LDAStatistics::pop_partial_slice(std::unique_ptr<LDAStatistics>& partial_stat){
    std::vector<int> slice;
    if(cur+slice_size < slice_.size())
      slice = std::vector<int>(slice_.begin()+cur, slice_.begin()+cur+slice_size);
    else
      slice = std::vector<int>(slice_.begin()+cur, slice_.end());
    partial_stat.reset(new LDAStatistics(K_, ndt_, slice, t_, d_, w_));
    cur += slice_size;
    if(slice_size - cur < slice_size)
      return -1;
    return 1;
  }

}
