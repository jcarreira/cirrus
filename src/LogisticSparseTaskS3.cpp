#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "S3SparseIterator.h"

#ifdef USE_EFS
#include "NFSls.h"
#include "NFSFile.h"
#endif

#include <pthread.h>

#define DEBUG

namespace cirrus {

#ifdef USE_EFS
void LogisticSparseTaskS3::push_gradient(LRSparseGradient* lrg, uint64_t version, uint64_t& grad_cum_size) {
  // here we write this gradient to the file gradient<gradient_version>
  std::string grad_name = "gradient" + std::to_string(version);
  NFSFile file(nfs_, grad_name);

  int32_t serialized_size = static_cast<int>(lrg->getSerializedSize());
  std::shared_ptr<char[]> data(new char[serialized_size]);
  lrg->serialize(data.get()); // FIXME this should called serializeTo for consistency

  std::cout << "serialized size: " << serialized_size << " at offset " << grad_cum_size << std::endl;
  int ret = file.write(grad_cum_size,
      reinterpret_cast<char*>(&serialized_size), sizeof(int32_t));
  ret = file.write(grad_cum_size + sizeof(int32_t), data.get(), serialized_size);
  if (ret != serialized_size) {
    throw std::runtime_error("Error writing gradient: " + grad_name);
  }

  grad_cum_size += sizeof(int32_t) + serialized_size;
}
#else
void LogisticSparseTaskS3::push_gradient(LRSparseGradient* lrg) {
}
#endif

// get samples and labels data
bool LogisticSparseTaskS3::get_dataset_minibatch(
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
        config.get_minibatch_size())); // this takes 11 us

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


#ifdef USE_EFS
class Comparator {
  public:
    Comparator(const std::string& str)
      : str(str) {}

    bool operator()(
        const std::pair<std::string, uint64_t>& p1,
        const std::pair<std::string, uint64_t>& p2) const {
      assert(strncmp(p1.first.c_str(), "model", strlen("model")) == 0);
      assert(strncmp(p2.first.c_str(), "model", strlen("model")) == 0);
      uint64_t num1 = string_to<uint64_t>(p1.first.substr(5));
      uint64_t num2 = string_to<uint64_t>(p2.first.substr(5));

      //std::cout << p1.first << " " << num1 << " -- " << p2.first << " " << num2 << std::endl;

      return num1 < num2;
    }
  private:
    std::string str;
};

/**
  * Find latest model file and read it
  */
void LogisticSparseTaskS3::get_latest_model(
    const SparseDataset& dataset,
    SparseLRModel& model,
    const Configuration& config) {
  
  NFSls ls(nfs_, "/");
  std::vector<std::pair<std::string, uint64_t>> all_files = ls.do_ls();
  std::vector<std::pair<std::string, uint64_t>> model_entries;

  // find all files that start with "model"
  auto it = std::copy_if (
      all_files.begin(), all_files.end(), std::back_inserter(model_entries),
      [](std::pair<std::string, uint64_t> p) {
        return strncmp(p.first.c_str(), "model", strlen("model")) == 0;
      });

  if (model_entries.size() == 0) {
    throw std::runtime_error("No model file found");
  }

  // sort all entries by model version
  std::sort(model_entries.begin(), model_entries.end(), Comparator("model"));

  std::cout << "Printing model entries" << std::endl;
  for (const auto& entry : model_entries) {
    std::cout << entry.first << " ";
  }
  std::cout << std::endl;

  auto first = model_entries.back();
  std::cout
    << "Earliest model is in file: " << first.first
    << " with size: " << first.second
    << std::endl;

  NFSFile file(nfs_, "/" + first.first);
  int ret = 0;
  
  uint32_t max_model_size = 10*1024*1024;
  std::shared_ptr<char[]> latest_model(new char[max_model_size]);
  ret = file.read(0, latest_model.get(), max_model_size);
  if (ret <= 0) { // 0 means we read the whole thing
    throw std::runtime_error("Error reading model. ret: " + std::to_string(ret));
  }
  model.loadSerialized(latest_model.get()); // update the model

  std::cout << "Latest model checksum: " << model.checksum() << std::endl;
}
#else
void LogisticSparseTaskS3::get_latest_model(
    const SparseDataset& dataset,
    SparseLRModel& model,
    const Configuration& config) {}
#endif

void LogisticSparseTaskS3::start() {
    nfs_ = nfs_init_context();
    if (nfs_ == NULL) {
        throw std::runtime_error("Error initing nfs");
    }

    url_ = nfs_parse_url_dir(nfs_,
            "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
    if (url_ == NULL) {
        throw std::runtime_error("Error parsing nfs url");
    }

    server_ = url_->server;
    path_ = url_->path;

    std::cout << "Mounting nfs" << std::endl;
    int ret = nfs_mount(nfs_, server_, path_);
    if (ret != 0) {
        throw std::runtime_error("Error mounting nfs ret: " + std::to_string(ret));
    }
}

void LogisticSparseTaskS3::test() {
  while (1) {
    NFSls ls(nfs_, "/");
    std::vector<std::pair<std::string, uint64_t>> all_files = ls.do_ls();
  }

}

void LogisticSparseTaskS3::run(const Configuration& config, int worker) {
  std::cout << "Starting LogisticSparseTaskS3"
    << std::endl;
  uint64_t num_s3_batches = config.get_limit_samples() / config.get_s3_size();
  this->config = config;

  start();
  //test();

  std::cout << "[WORKER] " << "num s3 batches: " << num_s3_batches
    << std::endl;
  wait_for_start(worker, nworkers);

  // Create iterator that goes from 0 to num_s3_batches
  auto train_range = config.get_train_range();
  S3SparseIterator s3_iter(
      train_range.first, train_range.second,
      config, config.get_s3_size(), config.get_minibatch_size(),
      true, worker);

  std::cout << "[WORKER] starting loop" << std::endl;

  uint64_t version = 1;
  SparseLRModel model(1 << config.get_model_bits());

  uint64_t grad_version = 0;
  uint64_t grad_cum_size = 0;

  bool printed_rate = false;
  int count = 0;
  auto start_time = get_time_ms();
  while (1) {
    // get data, labels and model
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] running phase 1" << std::endl;
#endif
    std::unique_ptr<SparseDataset> dataset;
    if (!get_dataset_minibatch(dataset, s3_iter)) {
      continue;
    }
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] phase 1 done. Getting the model" << std::endl;
    //dataset->check();
    //dataset->print_info();
    auto now = get_time_us();
#endif
    // compute mini batch gradient
    std::unique_ptr<ModelGradient> gradient;

    // we get the model subset with just the right amount of weights
    //sparse_model_get->get_new_model_inplace(*dataset, model, config);
    get_latest_model(*dataset, model, config);

#ifdef DEBUG
    std::cout << "get model elapsed(us): " << get_time_us() - now << std::endl;
    std::cout << "Checking model" << std::endl;
    //model.check();
    std::cout << "Computing gradient" << "\n";
    now = get_time_us();
#endif

    try {
#ifdef USE_EFS
      gradient = model.minibatch_grad(*dataset, config.get_epsilon());
      gradient->print();
#else
      gradient = model.minibatch_grad_sparse(*dataset, config);
#endif
    } catch(const std::runtime_error& e) {
      std::cout << "Error. " << e.what() << std::endl;
      exit(-1);
    } catch(...) {
      std::cout << "There was an error computing the gradient" << std::endl;
      exit(-1);
    }
#ifdef DEBUG
    auto elapsed_us = get_time_us() - now;
    std::cout << "[WORKER] Gradient compute time (us): " << elapsed_us
      << " at time: " << get_time_us()
      << " version " << version << "\n";
#endif
    gradient->setVersion(version++);

    try {
      LRSparseGradient* lrg = dynamic_cast<LRSparseGradient*>(gradient.get());
#ifdef USE_EFS
      auto t1 = get_time_us();
      push_gradient(lrg, grad_version, grad_cum_size);
      std::cout
        << "push gradient time (us): " << (get_time_us() - t1)
        << std::endl;
#else
      push_gradient(lrg);
#endif
    } catch(...) {
      std::cout << "[WORKER] "
        << "Worker task error doing put of gradient" << "\n";
      exit(-1);
    }
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] Sent gradient" << std::endl;
#endif
    count++;
    if (count % 10 == 0 && !printed_rate) {
      auto elapsed_ms = get_time_ms() - start_time;
      float elapsed_sec = elapsed_ms / 1000.0;
      if (elapsed_sec > (2 * 60)) {
        printed_rate = true;
        std::cout << "Update rate/sec last 2 mins: " << (1.0 * count / elapsed_sec) << std::endl;
      }
    }
  }
}

}  // namespace cirrus

