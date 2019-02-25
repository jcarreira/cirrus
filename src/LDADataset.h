#ifndef _LDADATASET_H_
#define _LDADATASET_H_

#include <string>
#include <vector>

namespace cirrus {
/**
 * This class is used to hold a LDA dataset
 */
class LDADataset {
 public:
  /**
   * Construct empty dataset
   */
  LDADataset();
  /**
   * Construct a dataset from a vector of documents
   * and a vector of vocabularies.
   * @ param docs: vector of pairs (word index, counts)
   * @ param vocabs: a vector of string
   */
  LDADataset(const std::vector<std::vector<std::pair<int, int>>>& docs,
             const std::vector<std::string>& vocabs,
             double sample_ratio);
  /**
   * Load a dataset from serialized format
   */
  LDADataset(const char* msg_begin, double sample_ratio);
  /**
   * Return both documents and vocabularies in serialized format
   */
  char* serialize();
  /**
   * Return the size of dataset in serialized format
   */
  int get_serialize_size();
  /**
   * Return partial documents (in vectors) of size equal sample_size
   */
  void get_some_docs_(std::vector<std::vector<std::pair<int, int>>>& docs);
  std::vector<std::vector<std::pair<int, int>>> get_some_docs();
  /**
   * @param s
   * Set the sample_size equal parameter s
   */
  void set_sample_size(int s) { sample_size = s; }
  /**
   * Return the corpus size
   */
  uint64_t num_docs() const;
  /**
   * Return the vocabulary space dimension
   */
  uint64_t num_vocabs() const;
  /**
   * Sanity check the dataset
   * whether word indices and counts are valid
   */
  void check() const;

 private:
  std::vector<std::vector<std::pair<int, int>>>
      docs_;                         //< a vector of (word_id, count) pairs
  std::vector<std::string> vocabs_;  //< a vector of words (in string)
  int sample_size = 0;               //< number of documents to sample (unused)
  int serialize_size = 0;            //< size of serialized LDADataset object
};
}  // namespace cirrus

#endif
