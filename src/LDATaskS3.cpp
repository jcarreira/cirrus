#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "S3SparseIterator.h"
#include "PSSparseServerInterface.h"

#include <pthread.h>

#undef DEBUG

namespace cirrus {

void LDATaskS3::push_gradient(LDAUpdates* gradient) {
  #ifdef DEBUG
    auto before_push_us = get_time_us();
    std::cout << "Publishing gradients" << std::endl;
  #endif
    psint->send_lda_update(*gradient);
  #ifdef DEBUG
    std::cout << "Published gradients!" << std::endl;
    auto elapsed_push_us = get_time_us() - before_push_us;
    static uint64_t before = 0;
    if (before == 0)
      before = get_time_us();
    auto now = get_time_us();
    std::cout << "[WORKER] "
        << "Worker task published gradient"
        << " with version: " << gradient->getVersion()
        << " at time (us): " << get_time_us()
        << " took(us): " << elapsed_push_us
        << " bw(MB/s): " << std::fixed <<
           (1.0 * gradient->getSerializedSize() / elapsed_push_us / 1024 / 1024 * 1000 * 1000)
        << " since last(us): " << (now - before)
        << "\n";
    before = now;
  #endif
}

// Get LDAStatistics from S3
bool LDATaskS3::get_dataset_minibatch(
      std::unique_ptr<LDAStatistics>& local_vars,
      S3SparseIterator& s3_iter) {
  #ifdef DEBUG
    auto start = get_time_us();
  #endif

    const void* minibatch = s3_iter.get_next_fast();
  #ifdef DEBUG
    auto finish1 = get_time_us();
  #endif
    local_vars.reset(new LDAStatistics(reinterpret_cast<const char*>(minibatch)));

  #ifdef DEBUG
    auto finish2 = get_time_us();
    double bw = 1.0 * local_vars->get_serialize_size() /
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

void LDATaskS3::run(const Configuration& config, int worker) {
    std::cout << "Starting LDATaskS3"
      << std::endl;
    uint64_t num_s3_batches = config.get_limit_samples() / config.get_s3_size();
    this->config = config;

    psint = new PSSparseServerInterface(ps_ip, ps_port);
    lda_model_get = std::make_unique<LDAModelGet>(ps_ip, ps_port);

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
    LDAModel model; // TODO pre-set the model size?

    bool printed_rate = false;
    int count = 0;
    auto start_time = get_time_ms();

    std::unique_ptr<LDAStatistics> s3_local_vars;
    get_dataset_minibatch(s3_local_vars, s3_iter);
    while (1) {
      // get data, labels and model
  #ifdef DEBUG
      std::cout << get_time_us() << " [WORKER] running phase 1" << std::endl;
  #endif
      std::unique_ptr<LDAStatistics> local_vars;
      // only gets partial LDAStatistics corresponding to one vocabulary slice
      if (s3_local_vars->pop_partial_slice(local_vars) == -1) {
        if(!get_dataset_minibatch(s3_local_vars, s3_iter))
          continue;
      }
  #ifdef DEBUG
      std::cout << get_time_us() << " [WORKER] phase 1 done. Getting the model" << std::endl;
      //dataset->check();
      //dataset->print_info();
      auto now = get_time_us();
  #endif
      // compute mini batch gradient
      std::unique_ptr<LDAUpdates> gradient;

      // we get the model subset with just the right amount of weights
      model = std::move(lda_model_get->get_new_model(*local_vars, config));

  #ifdef DEBUG
      std::cout << "get model elapsed(us): " << get_time_us() - now << std::endl;
      std::cout << "Checking model" << std::endl;
      //model.check();
      std::cout << "Computing gradient" << "\n";
      now = get_time_us();
  #endif

      try {
        gradient = model.sample_model();
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
        LDAUpdates* lrg = dynamic_cast<LDAUpdates*>(gradient.get());
        push_gradient(lrg);
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

      // TODO: need some method to store all the local changes
    }

    // TODO: push the local change to S3
}

} // namespace cirrus
