#include <LDADataset.h>
#include <iostream>
#include <string.h>
#include <Utils.h>

// #define MAX_MSG_SIZE (1024 * 1024 * 100)

namespace cirrus {
LDADataset::LDADataset() {}

LDADataset::LDADataset(std::vector<std::vector<std::pair<int, int> > > docs,
                       std::vector<std::string> vocabs) {
  docs_ = docs;
  vocabs_ = vocabs;
  sample_size = (docs_.size()) / 100;
}

LDADataset::LDADataset(const char* msg_begin) {
  std::vector<std::pair<int, int> > doc;
  const int* D = reinterpret_cast<const int*>(msg_begin);
  msg_begin = reinterpret_cast<const char*>(
      reinterpret_cast<const char*>(msg_begin) + sizeof(int));

  std::cout << "# of documents: " << *D << std::endl;
  for (int i = 0; i < *D; ++i) {
    const int* N = reinterpret_cast<const int*>(msg_begin);
    msg_begin = reinterpret_cast<const char*>(
        reinterpret_cast<const char*>(msg_begin) + sizeof(int));
    for (int j = 0; j < *N; ++j) {
      const int* word = reinterpret_cast<const int*>(msg_begin);
      msg_begin = reinterpret_cast<const char*>(
          reinterpret_cast<const char*>(msg_begin) + sizeof(int));
      const int* count = reinterpret_cast<const int*>(msg_begin);
      msg_begin = reinterpret_cast<const char*>(
          reinterpret_cast<const char*>(msg_begin) + sizeof(int));
      doc.push_back(std::make_pair(*word, *count));
    }
    docs_.push_back(doc);
    doc.clear();
  }
  std::string vocab;
  const int* V = reinterpret_cast<const int*>(msg_begin);
  msg_begin = reinterpret_cast<const char*>(
      reinterpret_cast<const char*>(msg_begin) + sizeof(int));
  for (int i = 0; i < *V; ++i) {
    const int* len = reinterpret_cast<const int*>(msg_begin);
    msg_begin = reinterpret_cast<const char*>(
        reinterpret_cast<const char*>(msg_begin) + sizeof(int));
    vocabs_.push_back(std::string(msg_begin, msg_begin + *len));
    msg_begin = reinterpret_cast<const char*>(
        reinterpret_cast<const char*>(msg_begin) + *len);
  }
  sample_size = (docs_.size()) / 100;
}

uint64_t LDADataset::num_docs() const {
  return docs_.size();
}

uint64_t LDADataset::num_vocabs() const {
  return vocabs_.size();
}

void LDADataset::check() const {
  for (const auto& w : docs_) {
    for (const auto v : w) {
      if (v.first < 0 || v.second <= 0)
        throw std::runtime_error("Input error");
    }
  }
  std::cout << "Dataset has been checked.\n";
}

void LDADataset::get_some_docs(
    std::vector<std::vector<std::pair<int, int> > >& docs) {
  if (docs_.size() > sample_size)
    docs.resize(sample_size);
  else
    docs.resize(docs_.size());
  std::copy(
      docs_.begin(),
      docs_.size() > sample_size ? docs_.begin() + sample_size : docs_.end(),
      docs.begin());
  docs_.erase(docs_.begin(), docs_.size() > sample_size
                                 ? docs_.begin() + sample_size
                                 : docs_.end());
}

char* LDADataset::serialize() {
  char* msg = new char[get_serialize_size()];
  char* msg_begin = msg;  // need to keep this pointer to delete later

  uint32_t D = 0, N = 0, V_letter = 0, V_word = 0;
  store_value<int>(msg, docs_.size());
  for (const auto& sample : docs_) {
    store_value<int>(msg, sample.size());
    for (const auto& w : sample) {
      store_value<int>(msg, w.first);
      store_value<int>(msg, w.second);
      ++N;
    }
    ++D;
  }

  store_value<int>(msg, vocabs_.size());
  for (const auto& vocab : vocabs_) {
    char* a = new char[vocab.length()];
    strcpy(a, vocab.c_str());
    store_value<int>(msg, strlen(a));
    store_value<char>(msg, a, strlen(a));

    ++V_word;
    V_letter += strlen(a);
  }

  serialize_size =
      (2 * N + D + V_word + 2) * sizeof(int) + V_letter * sizeof(char);
  return msg_begin;
}

int LDADataset::get_serialize_size() {
  if (serialize_size) {
    return serialize_size;
  }
  int D = docs_.size(), V = vocabs_.size(), N = 0, V_letter = 0;
  for (const auto& sample : docs_) {
    for (const auto& w : sample) {
      ++N;
    }
  }
  for (const auto& vocab : vocabs_) {
    V_letter += strlen(vocab.c_str());
  }
  serialize_size = (2 * N + D + V + 2) * sizeof(int) + V_letter * sizeof(char);
  return serialize_size;
}
}
