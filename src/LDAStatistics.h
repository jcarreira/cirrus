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
  char* serialize(uint64_t& to_send_size);
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
    * @param s
    *
    * Set the slice_size to s
    */
  void set_slice_size(int s) { slice_size = s; }
  /**
    * Copied the updated local variables from LDAModel
    */
  void store_new_stats(LDAModel& model);

  void get_ndt(std::vector<std::vector<int> >& ndt) { ndt = ndt_; }
  void get_slice(std::vector<int>& slice) { slice = slice_; }
  int get_slice_size() { return slice_.size(); }

  int current = 0;

  // private:
  /**
    *
    * @variable K_: # of potential topics
    * @variable ndt_: the statistics of word counts over documents and topics
    *             - size: D * K where D is the size of local corpus
    * @variable t_: the current assigned latent topics for each words in the
    *corpus
    *           d_: the document id for each words in the corpus
    *           w_: the global word id for each words in the corpus
    *             - size: # of words in the local corpus
    * @variable slice_: the local vocabulary space
    */
  int K_, slice_size = 1000;
  std::vector<std::vector<int> > ndt_;
  std::vector<int> slice_, t_, d_, w_;
};
}

#endif
