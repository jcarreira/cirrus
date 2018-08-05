#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "S3SparseIterator.h"
#include "PSSparseServerInterface.h"
#include "S3Client.h"

#include <pthread.h>

#undef DEBUG

namespace cirrus {

void LDATaskS3::push_gradient(LDAUpdates* gradient) {
#ifdef DEBUG
  auto before_push_us = get_time_us();
  std::cout << "Publishing gradients" << std::endl;
#endif
  psint->send_lda_update(*gradient);
// lda_model_get->send_lda_update(gradient);
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
            << " took(us): " << elapsed_push_us << " bw(MB/s): " << std::fixed
            << (1.0 * gradient->getSerializedSize() / elapsed_push_us / 1024 /
                1024 * 1000 * 1000)
            << " since last(us): " << (now - before) << "\n";
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
  double bw = 1.0 * local_vars->get_serialize_size() / (finish2 - start) *
              1000.0 * 1000 / 1024 / 1024;
  std::cout << "[WORKER] Get Sample Elapsed (S3) "
            << " minibatch size: " << config.get_minibatch_size()
            << " part1(us): " << (finish1 - start)
            << " part2(us): " << (finish2 - finish1) << " BW (MB/s): " << bw
            << " at time: " << get_time_us() << "\n";
#endif
  return true;
}

void LDATaskS3::run(const Configuration& config, int worker) {

  double lambda_time_out = 3.0 * 60.0; // 3 min currently

  std::cout << "Starting LDATaskS3" << std::endl;
  uint64_t num_s3_batches = config.get_limit_samples() / config.get_s3_size();
  this->config = config;

  psint = new PSSparseServerInterface(ps_ip, ps_port);
  psint->connect();

  std::cout << "[WORKER] "
            << "num s3 batches: " << num_s3_batches << std::endl;
  wait_for_start(worker, nworkers);

  auto train_range = config.get_train_range();
  int range_per_worker =
      (train_range.second - train_range.first + 1) / nworkers;
  int start = worker * range_per_worker + 1,
      end = (worker + 1) * range_per_worker + 1;
  if (end > train_range.second) {
    end = train_range.second;
  }


  int cur_train_idx = start;

  std::cout << "[WORKER] starting loop" << std::endl;

  uint64_t version = 1;
  std::unique_ptr<LDAModel> model;

  bool printed_rate = false;
  int count = 0;
  auto start_time = get_time_ms();
  std::shared_ptr<LDAStatistics> s3_local_vars;

  s3_initialize_aws();
  std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();
  std::string obj_id_str =
      std::to_string(hash_f(std::to_string(cur_train_idx).c_str())) + "-LDA";
  std::ostringstream* s3_obj =
      s3_client->s3_get_object_ptr(obj_id_str, config.get_s3_bucket());
  s3_local_vars.reset(new LDAStatistics(s3_obj->str().c_str()));
  s3_local_vars->set_slice_size(config.get_slice_size());
  delete s3_obj;
  s3_shutdown_aws();

  int update_bucket = 0, stuck = 1;

  while (1) {

#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] running phase 1" << std::endl;
#endif
    std::unique_ptr<LDAStatistics> local_vars;

    if (s3_local_vars->pop_partial_slice(local_vars) == -1) {

      char* mem = s3_local_vars->serialize();

      s3_initialize_aws();
      std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();

      s3_client->s3_put_object(
          obj_id_str, config.get_s3_bucket(),
          std::string(mem,
                      s3_local_vars->get_serialize_size()));

      delete[] mem;

      auto elapsed_ms = get_time_ms() - start_time;
      float elapsed_sec = elapsed_ms / 1000.0;
      if (elapsed_sec > (lambda_time_out - 20.0)) {
        std::cout << "successfully exit\n";
        break;
      }


      update_bucket = cur_train_idx;

      cur_train_idx += 1;
      if (cur_train_idx == end) {
        cur_train_idx = start;
      }

      obj_id_str =
          std::to_string(hash_f(std::to_string(cur_train_idx).c_str())) +
          "-LDA";

      std::ostringstream* s3_obj =
          s3_client->s3_get_object_ptr(obj_id_str, config.get_s3_bucket());
      s3_local_vars.reset(new LDAStatistics(s3_obj->str().c_str()));
      s3_local_vars->set_slice_size(config.get_slice_size());

      delete s3_obj;
      s3_shutdown_aws();

      continue;
    }

#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] phase 1 done. Getting the model"
              << std::endl;
    auto now = get_time_us();
#endif
    // compute mini batch gradient
    std::unique_ptr<LDAUpdates> gradient;

    // we get the model subset with just the right amount of weights
    // TODO can be improved
    psint->get_lda_model(*local_vars, update_bucket, model);
    // psint->get_lda_model(*local_vars, update_bucket, model);
    // if (stuck == -1)
    //   continue;

    if (update_bucket != 0) {
      update_bucket = 0;
    }

#ifdef DEBUG
    std::cout << "get model elapsed(us): " << get_time_us() - now << std::endl;
    std::cout << "Checking model" << std::endl;
    // model.check();
    std::cout << "Computing gradient"
              << "\n";
    now = get_time_us();
#endif

    try {
      gradient = model->sample_model();
    } catch (const std::runtime_error& e) {
      std::cout << "Error. " << e.what() << std::endl;
      exit(-1);
    } catch (...) {
      std::cout << "There was an error computing the gradient" << std::endl;
      exit(-1);
    }
    s3_local_vars->store_new_stats(*model.get());

#ifdef DEBUG
    auto elapsed_us = get_time_us() - now;
    std::cout << "[WORKER] Gradient compute time (us): " << elapsed_us
              << " at time: " << get_time_us() << " version " << version
              << "\n";
#endif
    gradient->setVersion(version++);

    try {
      push_gradient(gradient.get());
    } catch (...) {
      std::cout << "[WORKER] "
                << "Worker task error doing put of gradient"
                << "\n";
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
        std::cout << "Update rate/sec last 2 mins: "
                  << (1.0 * count / elapsed_sec) << std::endl;
      }
    }
  }
}

}  // namespace cirrus
