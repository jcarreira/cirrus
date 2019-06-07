#include "S3NPIterator.h"
#include "Utils.h"
#include <unistd.h>
#include <vector>
#include <iostream>

#include <pthread.h>

//#define DEBUG

namespace cirrus {
  
// s3_cad_size nmber of samples times features per sample
S3NPIterator::S3NPIterator(uint64_t left_id,
                           uint64_t right_id,  // right id is exclusive
                           const Configuration& c,
                           uint64_t s3_rows,
                           uint64_t minibatch_rows,
                           bool use_label,
                           int worker_id,
                           bool random_access,
                           bool has_labels)
    : S3Iterator(c, has_labels),
      left_id(left_id),
      right_id(right_id),
      s3_rows(s3_rows),
      minibatch_rows(minibatch_rows),
      use_label(use_label),
      worker_id(worker_id),
      re(worker_id),
      random_access(random_access) {
  std::cout << "S3NPIterator::Creating S3NPIterator"
            << " left_id: " << left_id << " right_id: " << right_id
            << " use_label: " << use_label << " has_labels: " << has_labels
            << std::endl;

  // initialize s3
  s3_client = std::make_shared<S3Client>();

  // we fix the random seed but make it different for every worker
  // to ensure each worker receives a different minibatch
  if (random_access) {
    srand(42 + worker_id);
  } else {
    current = left_id;
  }
}

static int sstreamSize(std::ostringstream& ss) {
  return ss.tellp();
}

uint64_t S3NPIterator::getObjId(uint64_t left, uint64_t right) {
  if (random_access) {
    std::uniform_int_distribution<int> sampler(left, right - 1);
    uint64_t sampled = sampler(re);
    //uint64_t sampled = rand() % right;
    std::cout << "Sampled : " << sampled << " worker_id: " << worker_id << " left: " << left << " right: " << right << std::endl;
    return sampled;
  } else {
    auto ret = current++;
    if (current == right_id)
      current = left_id;
    return ret;
  }
}

std::shared_ptr<SparseDataset> S3NPIterator::getNext() {
  std::string obj_id_str = std::to_string(getObjId(left_id, right_id));
  std::ostringstream* s3_obj;
    
  std::cout << "S3NPIterator: getting object " << obj_id_str;
  uint64_t start = get_time_us();
  s3_obj = s3_client->s3_get_object_ptr(obj_id_str, config.get_s3_bucket());
  uint64_t elapsed_us = (get_time_us() - start);
  double mb_s = sstreamSize(*s3_obj) / elapsed_us * 1000.0 * 1000 / 1024 / 1024;
  std::cout << "received s3 obj"
            << " elapsed: " << elapsed_us
            << " size: " << sstreamSize(*s3_obj) << " BW (MB/s): " << mb_s
            << "\n";
  
  uint64_t n_minibatches = s3_rows / minibatch_rows;
  std::string str = s3_obj->str();
  const void* s3_data = str.c_str();
  //delete s3_obj;
  int s3_obj_size = load_value<int>(s3_data);
  int num_samples = load_value<int>(s3_data);
  std::cout << s3_obj_size << "-" << num_samples << std::endl;
  assert(s3_obj_size > 0 && s3_obj_size < 100 * 1024 * 1024);
  assert(num_samples > 0 && num_samples < 1000000);

  //for (uint64_t i = 0; i < n_minibatches; ++i) {
  //  // if it's the last minibatch in object we mark it so it can be deleted
  //  //new_queue->push(std::make_pair(s3_data, is_last));
  //  // advance ptr sample by sample
  //  for (uint64_t j = 0; j < minibatch_rows; ++j) {
  //    if (use_label) {
  //      FEATURE_TYPE label = load_value<FEATURE_TYPE>(s3_data); // read label
  //      std::cout << "label:" << label << std::endl;
  //      assert(label == 0.0 || label == 1.0);
  //    }
  //    int num_values = load_value<int>(s3_data); 
  //    std::cout << "num_values:" << num_values << std::endl;
  //    assert(num_values >= 0 && num_values < 1000000);
  //    advance_ptr(s3_data, num_values * (sizeof(int) + sizeof(FEATURE_TYPE)));
  //  }
  //}

  std::shared_ptr<SparseDataset> ds =
      std::make_shared<SparseDataset>(reinterpret_cast<const char*>(s3_data),
                                      config.get_minibatch_size(), has_labels);

  ds->check();

  return ds;
}

} // namespace cirrus
