#include <Tasks.h>

#include <array>
#include <random>
#include <set>
#include "InputReader.h"
#include "S3.h"
#include "Serializers.h"
#include "Utils.h"
#include "config.h"

namespace cirrus {

#define READ_INPUT_THREADS (10)

LDADataset LoadingLDATaskS3::read_dataset(const Configuration& config) {
  InputReader input;
  return input.read_lda_input(config.get_doc_path(), config.get_vocab_path(),
                              ",", config);
}

LDAStatistics LoadingLDATaskS3::count_dataset(
    const std::vector<std::vector<std::pair<int, int> > >& docs,
    std::vector<int>& nvt,
    std::vector<int>& nt,
    std::vector<int>& w,
    int K,
    std::vector<int>& global_vocab,
    std::vector<std::vector<int> >& topic_scope) {
  std::vector<int> d, t;
  std::vector<std::vector<int> > ndt;
  std::vector<int> local_vocab;

  std::array<int, VOCAB_DIM_UPPER> lindex_map;
  lindex_map.fill(-1);

  for (const auto& doc : docs) {
    std::vector<int> ndt_row(K);

    for (const auto& feat : doc) {
      int gindex = feat.first, count = feat.second;
      if (lindex_map[gindex] == -1) {
        local_vocab.push_back(gindex);
        lindex_map[gindex] = 1;
      }
      if (lookup_map[gindex] == -1) {
        global_vocab.push_back(gindex);
        lookup_map[gindex] = idx;
        idx++;
      }
      for (int i = 0; i < count; ++i) {
        int top = topic_scope[gindex][(rand() % topic_scope[gindex].size())];

        t.push_back(top);
        d.push_back(ndt.size());
        w.push_back(gindex);
        ndt_row[top] += 1;

        // ensure the i^{th} row in nvt-stat corresponds to the i^{th} word
        // in the slice
        nvt[lookup_map[gindex] * K + top] += 1;
        nt[top] += 1;
      }
    }
    ndt.push_back(ndt_row);
  }
  return LDAStatistics(K, ndt, local_vocab, t, d, w);
}

void LoadingLDATaskS3::run(const Configuration& config) {
  std::cout << "[LOADER] "
            << "Read lda input..." << std::endl;

  lookup_map.fill(-1);

  uint64_t s3_obj_num_samples = config.get_s3_file_size();
  std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();

  int K = config.get_k();

  LDADataset dataset = read_dataset(config);
  dataset.check();
  dataset.set_sample_size(s3_obj_num_samples);

  uint64_t num_s3_objs = dataset.num_docs() / s3_obj_num_samples;
  if (num_s3_objs * s3_obj_num_samples != dataset.num_docs()) {
    num_s3_objs += 1;
  }

  // num_s3_objs number of partial doc models (assigned to workers)
  // and one vocab model (received by the server)
  std::cout << "[LOADER-SPARSE] "
            << "Adding " << dataset.num_docs()
            << " #s3 objs: " << num_s3_objs + 1
            << " bucket: " << config.get_s3_bucket() << std::endl;

  std::vector<int> nvt, nt;
  std::vector<std::vector<int> > ws;

  nvt.resize(dataset.num_vocabs() * K);
  nt.resize(K);
  ws.reserve(num_s3_objs);

  std::vector<std::vector<int> > nvt_init_rnd_scope;
  std::vector<int> temp_global_vocab;
  temp_global_vocab.reserve(K);
  for (int i = 0; i < K; ++i) {
    temp_global_vocab.push_back(i);
  }
  int length = K;
  nvt_init_rnd_scope.reserve(dataset.num_vocabs());
  for (int i = 0; i < dataset.num_vocabs(); ++i) {
    std::random_shuffle(temp_global_vocab.begin(), temp_global_vocab.end());
    std::vector<int> vi_init_scope(temp_global_vocab.begin(),
                                   temp_global_vocab.begin() + length);
    nvt_init_rnd_scope.push_back(vi_init_scope);
  }

  std::vector<int> global_vocab;
  uint64_t to_send_size;

  // Storing local variables (LDAStatistics)
  for (unsigned int i = 1; i < num_s3_objs + 1; ++i) {
    std::vector<int> w;

    std::cout << "[LOADER] Building s3 batch #" << i << std::endl;

    // Only get corpus of size s3_obj_num_samples
    std::vector<std::vector<std::pair<int, int> > > partial_docs;
    dataset.get_some_docs(partial_docs);

    LDAStatistics to_save = count_dataset(partial_docs, nvt, nt, w, K,
                                          global_vocab, nvt_init_rnd_scope);
    std::cout << i << " : " << w.size() << std::endl;
    ws.push_back(w);

    char* msg = to_save.serialize(to_send_size);

    std::cout << "Putting object(LDAStatistics) in S3 with size: "
              << to_send_size << std::endl;
    std::string obj_id =
        std::to_string(hash_f(std::to_string(SAMPLE_BASE + i).c_str())) +
        "-LDA";
    s3_client->s3_put_object(obj_id, config.get_s3_bucket(),
                             std::string(msg, to_send_size));
    delete msg;
  }

  std::shared_ptr<LDAUpdates> initial_global_var;
  initial_global_var.reset(new LDAUpdates());
  initial_global_var->slice = global_vocab;
  initial_global_var->change_nvt_ptr.reset(new std::vector<int>(nvt));
  initial_global_var->change_nt_ptr.reset(new std::vector<int>(nt));
  initial_global_var->ws_ptr.reset(new std::vector<std::vector<int> >(ws));

  // LDAUpdates initial_global_var(nvt, nt, global_vocab_vec);
  std::cout << "Putting object(initial global var) in S3 with size: "
            << initial_global_var->getSerializedSize() << std::endl;
  std::string obj_id =
      std::to_string(hash_f(std::to_string(SAMPLE_BASE).c_str())) + "-LDA";

  uint32_t len;
  std::shared_ptr<char> s3_obj = initial_global_var->serialize(&len);

  s3_client->s3_put_object(obj_id, config.get_s3_bucket(),
                           std::string(s3_obj.get(), len));

  std::cout << "LOADER-LDA terminated successfully" << std::endl;
}

}  // namespace cirrus
