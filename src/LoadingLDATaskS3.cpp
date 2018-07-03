#include <Tasks.h>

#include "Serializers.h"
#include "InputReader.h"
#include "S3.h"
#include "Utils.h"
#include "config.h"
#include <random>

namespace cirrus {

#define READ_INPUT_THREADS (10)

LDADataset LoadingSparseTaskS3::read_dataset(
    const Configuration& config) {

  InputReader input;
  return input.read_lda_input(
      config.get_input_path(),
      // config.get_vocab_path(),
      ",",
      config);
}

LDAStatistics LoadingSparseTaskS3::count_dataset(
                    const std::vector<std::vector<std::pair<int, int>>>& docs,\
                    std::vector<int>& nvt,
                    std::vector<int>& nt, int K){

  std::vector<int> t, d, w;
  std::vector<std::vector<int>> ndt;
  std::set<int> local_vocab
  std::vector,int> ndt_row(K);

  for(const auto& doc: docs){
    for(const auto& feat: doc){
      int gindex = feat.first, count = feat.second;
      if(local_vocab.find(gindex) == local_vocab.end())
        local_vocab.insert(local_vocab.begin(), gindex);
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
    ++ idx;
    ndt_row.clear();
  }
  std::vector<int> local_vocab_vec(local_vocab.begin(), local_vocab.end());
  return LDAStatistics(K, ndt, local_vocab_vec, t, d, w);
}

void LoadingSparseTaskS3::run(const Configuration& config) {
  std::cout << "[LOADER] " << "Read lda input..." << std::endl;

  uint64_t s3_obj_num_samples = config.get_s3_size();
  s3_initialize_aws();
  auto s3_client = s3_create_client();

  int K = config.get_k();

  LDADataset dataset = read_dataset(config);
  dataset.check();
  dataset.set_sample_size(s3_obj_num_samples);

  uint64_t num_s3_objs = dataset.num_docs() / s3_obj_num_samples;
  std::cout << "[LOADER-SPARSE] "
    << "Adding " << dataset.num_docs()
    << " #s3 objs: " << num_s3_objs + 2
    << " bucket: " << config.get_s3_bucket()
    << std::endl;

  std::vector<int> nvt, nt;
  std::vector<std::vector<std::pair<int, int>>> partial_docs;
  nvt.resize(dataset.num_vocabs() * K);
  nt.resize(K);

  // Storing local variables (LDAStatistics)
  for (unsigned int i = 1; i < num_s3_objs + 1; ++i) {
    std::cout << "[LOADER] Building s3 batch #" << (i + 1) << std::endl;

    // Only get corpus of size s3_obj_num_samples
    dataset.get_some_docs(partial_docs);
    LDAStatistics to_save = count_dataset(partial_docs, nvt, nt, K);

    std::cout << "Putting object(LDAStatistics) in S3 with size: " << to_save.get_serialize_size() << std::endl;
    std::string obj_id = std::to_string(hash_f(std::to_string(SAMPLE_BASE + i).c_str())) + "-LDA";
    s3_put_object(obj_id, s3_client, config.get_s3_bucket(),
        std::string(to_save.serialize(), to_save.get_serialize_size()));
  }
  // check_loading(config, s3_client);
  
  // Storing global variables
  LDAUpdates initial_global_var(nvt, nt);
  std::cout << "Putting object(initial global var) in S3 with size: " << initial_global_var.getSerializedSize() << std::endl;
  std::string obj_id = std::to_string(hash_f(std::to_string(SAMPLE_BASE).c_str())) + "-LDA";
  s3_put_object(obj_id, s3_client, config.get_s3_bucket(),
      std::string(initial_global_var.serialize(), initial_global_var.getSerializedSize()));

  std::cout << "LOADER-LDA terminated successfully" << std::endl;
}

} // namespace cirrus
