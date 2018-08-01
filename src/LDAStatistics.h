#ifndef _LDASTATISTICS_H_
#define _LDASTATISTICS_H_

#include <vector>
#include <string>
#include <memory>
#include <set>
#include "LDAModel.h"
namespace cirrus {
/**
  * This class is used to hold local variables
  *                            (ndt, topic assignment, slice)
  */
class LDAStatistics {
 public:
  /**
    * Create an empty LDAStatistics
    */
  LDAStatistics();
  /**
    * Load LDAStatistics from serialized memory
    */
  LDAStatistics(const char* msg);
  /**
    * Create a LDAStatistics based on vectors.
    * Construction is done by simply copying vectors.
    */
  LDAStatistics(int K,
                std::vector<std::vector<int> >& ndt,
                std::vector<int>& slice,
                std::vector<int>& t,
                std::vector<int>& d,
                std::vector<int>& w);

  /**
    * Cast all the local variables into memory
    */
  char* serialize();
  /**
    * Only cast vocabulary slice into memory
    */
  char* serialize_slice();
  /**
    * Get the size of serialized memory containing all information
    */
  int get_serialize_size();
  /**
    * Get the size of serialized memory only containing vocabulary slice
    */
  int get_serialize_slice_size();
  /**
    * Get the size of serialized memory containing partial global variables
    * needed for sampling this LDAStatistics.
    *
    * The serialized memory is sent from server.
    */
  int get_receive_size();
  /**
    * Get the number of documents stored in this LDAStatistics
    */
  int get_num_docs() const { return ndt_.size(); }
  /**
    * @param minibatch_size
    *
    * Return partial LDAStatistics corresponding to minibatch_size
    * number of documents stored in this LDAStatistics
    */
  // char* pop_partial_docs(int minibatch_size);
  /**
    * @param s
    *
    * Set the slice_size to s
    */
  void set_slice_size(int s) { slice_size = s; }
  /**
    * Return partial LDAStatistics covering only slice_size number of words
    */
  int pop_partial_slice(std::unique_ptr<LDAStatistics>& partial_stat);

  void store_new_stats(LDAModel& model);
  // void store_new_stats();

  void get_ndt(std::vector<std::vector<int> >& ndt) { ndt = ndt_; }
  void get_slice(std::vector<int>& slice) { slice = slice_; }
  void reset_current() { current = 0; }

 // private:
  int K_, slice_size = 1000, current = 0;
  std::vector<std::vector<int> > ndt_;
  std::vector<int> slice_, t_, d_, w_;
};
}

#endif
