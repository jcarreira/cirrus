#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "S3SparseIterator.h"
#include "PSSparseServerInterface.h"
#include "SparseMFModel.h"

#include <pthread.h>

#define DEBUG

namespace cirrus {

// get samples and labels data
bool MFNetflixTask::get_dataset_minibatch(
    std::unique_ptr<SparseDataset>& dataset,
    S3SparseIterator& s3_iter) {
#ifdef DEBUG
  auto start = get_time_us();
#endif

  const void* minibatch = s3_iter.get_next_fast();
#ifdef DEBUG
  auto finish1 = get_time_us();
#endif
  dataset.reset(new SparseDataset(reinterpret_cast<const char*>(minibatch),
        config.get_minibatch_size(), false));  // this takes 11 us

#ifdef DEBUG
  auto finish2 = get_time_us();
  double bw = 1.0 * dataset->getSizeBytes() /
    (finish2-start) * 1000.0 * 1000 / 1024 / 1024;
  std::cout << "[WORKER] Get Sample Elapsed (S3) "
    << " minibatch size: " << config.get_minibatch_size()
    << " part1(us): " << (finish1 - start)
    << " part2(us): " << (finish2 - finish1)
    << " BW (MB/s): " << bw
    << " at time: " << get_time_us()
    << "\n";
#endif
  return true;
}

std::vector<std::vector<std::tuple<uint32_t, uint32_t>>>
MFNetflixTask::buildIndexes(const SparseDataset& ds,
                             uint64_t sample_index,
                             uint32_t mb_size) {
  std::vector<std::vector<std::tuple<uint32_t, uint32_t>>> res;
  res.resize(4); // user_bias, item_bias, user_weights, item_weights

  bool seen[17770] = {false}; // fix
  for (const auto& sample : ds.data_) {
    for (const auto& w : sample) {
      uint32_t movieId = w.first;
      if (seen[movieId]) {
        continue;
      } else {
        seen[movieId] = true;
      }
      res[1].push_back(std::make_tuple(movieId, UINT32_MAX)); // get all values
      res[3].push_back(std::make_tuple(movieId, UINT32_MAX)); // get all values
    }
  }

  for (uint32_t i = sample_index; i < sample_index + mb_size; ++i) {
    res[0].push_back(std::make_tuple(i, UINT32_MAX));
    res[2].push_back(std::make_tuple(i, UINT32_MAX));
  }
}

void MFNetflixTask::run(const Configuration& config, int worker) {
  std::cout << "Starting MFNetflixTask"
            << std::endl;
  uint64_t num_s3_batches = config.get_limit_samples() / config.get_s3_size();
  this->config = config;

  psint = std::make_unique<PSSparseServerInterface>(ps_ip, ps_port);
  psint->createTensor1D("mf_item_bias", 0);
  psint->createTensor1D("mf_user_bias", 0);
  psint->createTensor2D("mf_item_weights", std::make_pair(0, 0));
  psint->createTensor2D("mf_user_weights", std::make_pair(0, 0));

  std::cout << "[WORKER] " << "num s3 batches: " << num_s3_batches
    << std::endl;
  waitForStart(WORKER_SPARSE_TASK_RANK + worker, nworkers);

  // Create iterator that goes from 0 to num_s3_batches
  std::pair<int, int> train_range = config.get_train_range();

  /** We sequentially iterate over data
    * This is necessary because we need to know the index of each row
    * in the dataset matrix because that tells us which user it belongs to
    * (same doesn't happen with Logistic Regression)
    */

  int l = train_range.first;
  int r = train_range.second;
  uint64_t sample_low = 0;
  uint64_t sample_index = 0;
  uint64_t sample_high = config.get_s3_size() * (config.get_train_range().second + 1);

  if (config.get_netflix_workers()) {
    int range_length = (train_range.second - train_range.first) / config.get_netflix_workers();
    range_length += 1;

    l = worker * range_length;
    r = std::min(l + range_length, r);

    sample_low = l * config.get_s3_size();
    sample_high = std::min(sample_high, (r + 1) * config.get_s3_size());

    sample_index = sample_low;

  }

  S3SparseIterator s3_iter(
      l, r + 1, config, config.get_s3_size(), config.get_minibatch_size(),
      false, worker, false);

  std::cout << "[WORKER] starting loop" << std::endl;

  while (1) {
    // get data, labels and model
#ifdef DEBUG
    std::cout << "[WORKER] running phase 1" << std::endl;
#endif
    std::unique_ptr<SparseDataset> dataset;
    if (!get_dataset_minibatch(dataset, s3_iter)) {
      continue;
    }
    std::cout << "DS size: " << dataset->num_samples() << std::endl;
#ifdef DEBUG
    std::cout << "[WORKER] phase 1 done" << std::endl;
    dataset->check();
    dataset->print_info();
    auto now = get_time_us();
#endif
    // compute mini batch gradient
    std::unique_ptr<ModelGradient> gradient;

#if 0

    std::vector<std::vector<std::tuple<uint32_t, uint32_t>>> indexes =
      build_indexes(*dataset, sample_index, config.get_minibatch_size());
    // we get the model subset with just the right amount of weights
    SparseTensor1D mf_user_bias = psint->getSparseTensor1D("mf_user_bias",
                                                         indexes[0]);
    SparseTensor1D mf_item_bias = psint->getSparseTensor1D("mf_item_bias",
                                                         indexes[1]);
    SparseTensor2D mf_user_weights = psint->getSparseTensor2D("mf_user_weights",
                                                            indexes[2]);
    SparseTensor2D mf_item_weights = psint->getSparseTensor2D("mf_item_weights",
                                                            indexes[3]);

    SparseMFModel model(std::move(indexes[0]),
                        std::move(indexes[1]),
                        std::move(indexes[2]),
                        std::move(indexes[3]));
    //SparseMFModel model =
    //  mf_model_get->get_new_model(
    //          *dataset, sample_index, config.get_minibatch_size());

#ifdef DEBUG
    std::cout << "get model elapsed(us): " << get_time_us() - now << std::endl;
    std::cout << "Checking model" << std::endl;
    std::cout << "Computing gradient" << "\n";
    now = get_time_us();
#endif

    try {
      auto gradient_tensor = model.minibatchGradTensor(*dataset, config, sample_index);
      //auto gradient = model.minibatchGrad(*dataset, config, sample_index);
#ifdef DEBUG
      auto elapsed_us = get_time_us() - now;
      std::cout << "[WORKER] Gradient compute time (us): " << elapsed_us
        << " at time: " << get_time_us() << "\n";
#endif
      //MFSparseGradient* grad_ptr =
      //  dynamic_cast<MFSparseGradient*>(gradient.get());
      //push_gradient(*grad_ptr);

      // XXX fix
      //psint->add_tensor("mf_model", gradient_tensor);
      sample_index += config.get_minibatch_size();

      if (sample_index + config.get_minibatch_size() > sample_high) {
          sample_index = sample_low;
      }
    } catch(...) {
      std::cout << "There was an error computing the gradient" << std::endl;
      exit(-1);
    }
#endif
  }
}

} // namespace cirrus
