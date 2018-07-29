#ifndef _LDADATASET_H_
#define _LDADATASET_H_

#include <vector>
#include <string>

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
   */
  LDADataset(std::vector<std::vector<std::pair<int, int> > > docs,
             std::vector<std::string> vocabs);
  /**
    * Load a dataset from serialized format
    */
  LDADataset(const char* msg_begin);
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
  void get_some_docs(std::vector<std::vector<std::pair<int, int> > >& docs);
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
  std::vector<std::vector<std::pair<int, int> > > docs_;
  std::vector<std::string> vocabs_;
  int sample_size = 0, serialize_size = 0;
};
}

#endif
