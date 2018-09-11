#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "S3SparseIterator.h"
#include "PSSparseServerInterface.h"
#include "S3Client.h"

#include <pthread.h>
#include <thread>

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

// void LDATaskS3::create_lda_model(LDAStatistics& info, int update_bucket, char* buffer,
//                                  std::unique_ptr<LDAModel>& model,
//                                  uint32_t to_receive_size,
//                                  uint32_t uncompressed_size) {
//
//   // uint64_t to_send_size;
//   // char* msg_begin = info.serialize(to_send_size);
//
//   // model.reset(new LDAModel(buffer, msg_begin, update_bucket, to_receive_size, uncompressed_size));
//   // model.reset(new LDAModel(msg_begin));
//   model->update_model(buffer, update_bucket, to_receive_size, uncompressed_size);
//
//   // delete[] msg_slice_begin;
//   // delete[] msg_begin;
//   delete[] buffer;
// }

// void LDATaskS3::pre_fetch_with_bucket_fn(std::shared_ptr<LDAStatistics>& pre_fetch_vars,
//                                          std::mutex& pre_fetch_lock,
//                                          int bucket_id) {
//
//   pre_fetch_lock.lock();
//
//   std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();
//   std::string obj_id_str =
//        std::to_string(hash_f(std::to_string(bucket_id).c_str())) + "-LDA";
//   std::ostringstream* s3_obj =
//        s3_client->s3_get_object_ptr(obj_id_str, config.get_s3_bucket());
//
//   pre_fetch_vars.reset(new LDAStatistics(s3_obj->str().c_str()));
//   // pre_fetch_vars->set_slice_size(this->config.get_slice_size());
//   delete s3_obj;
//
//   pre_fetch_lock.unlock();
//
// }

void LDATaskS3::upload_wih_bucket_id_fn(std::shared_ptr<LDAStatistics> to_save,
                             int& upload_lock,
                             int bucket_id){

  while (true) {
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
    if (upload_lock == -1){
      upload_lock = 1;
      break;
    }
  }

  uint64_t to_send_size;
  char* mem = to_save->serialize(to_send_size);
  std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();
  std::string obj_id_str =
       std::to_string(hash_f(std::to_string(bucket_id).c_str())) + "-LDA";
  s3_client->s3_put_object(
       obj_id_str, this->config.get_s3_bucket(),
       std::string(mem, to_send_size));

  to_save.reset();

  upload_lock = -1;
  delete[] mem;

}

// void LDATaskS3::pre_fetch_model_fn(std::unique_ptr<LDAModel>& model,
//                                    std::unique_ptr<LDAStatistics>& local_vars,
//                                    int update_bucket,
//                                    bool& done) {
//
//   std::cout << local_vars->K_ << " ******\n";
//
//   pre_fetch_lock.lock();
//   psint->get_lda_model(*local_vars, update_bucket, model);
//   pre_fetch_lock.unlock();
//
//   done = true;
// }

void LDATaskS3::run(const Configuration& config, int worker) {

  double lambda_time_out = 300.0; // 3 min currently
  double time_download = 0.0, time_update = 0.0, time_get_model = 0.0, time_sample = 0.0,
         time_create_model = 0.0;

  std::cout << "Starting LDATaskS3" << std::endl;
  uint64_t num_s3_batches = config.get_limit_samples() / config.get_s3_size();
  this->config = config;

  psint = new PSSparseServerInterface(ps_ip, ps_port);
  psint->connect();

  std::cout << "[WORKER] "
            << "num s3 batches: " << num_s3_batches << std::endl;
  wait_for_start(worker, nworkers);


  // pre-assign the documents for workers
  auto train_range = config.get_train_range();
  int range_per_worker =
      (train_range.second - train_range.first + 1) / nworkers;
  int start = worker * range_per_worker + 1,
      end = (worker + 1) * range_per_worker + 1;
  if (end > train_range.second) {
    end = train_range.second;
  }

  upload_lock_indicators.resize(end - start, -1);
  // temp_locks.fill(-1);

  int cur_train_idx = start; //, pre_fetch_idx = start + 1;
  // if (pre_fetch_idx == end) {
  //   pre_fetch_idx = start;
  // }

  std::cout << "[WORKER] starting loop" << std::endl;

  uint64_t version = 1;
  std::unique_ptr<LDAModel> model, next_model;

  bool printed_rate = false;
  int count = 0;
  auto start_time = get_time_ms();
  std::shared_ptr<LDAStatistics> s3_local_vars; //, pre_fetch_vars;

  // Load the first LDAStatistics from S3
  auto start_time_benchmark = get_time_ms();
  s3_initialize_aws();
  std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();
  std::string obj_id_str =
      std::to_string(hash_f(std::to_string(cur_train_idx).c_str())) + "-LDA";
  std::ostringstream* s3_obj =
      s3_client->s3_get_object_ptr(obj_id_str, config.get_s3_bucket());
  s3_local_vars.reset(new LDAStatistics(s3_obj->str().c_str()));
  s3_local_vars->set_slice_size(config.get_slice_size());
  delete s3_obj;
  time_download += (get_time_ms() - start_time_benchmark) / 1000.0;

  std::unique_ptr<LDAStatistics> local_vars;
  // s3_local_vars->pop_partial_slice(local_vars);
  //
  // pre_fetch_model_threads.push_back(std::make_unique<std::thread>(
  //       std::bind(&LDATaskS3::pre_fetch_model_fn, this,
  //                 std::placeholders::_1, std::placeholders::_2,
  //                 std::placeholders::_3, std::placeholders::_4),
  //       std::ref(next_model), std::ref(local_vars), 0, std::ref(pre_fetch_done))
  // );
  //
  // s3_local_vars->incre_current();
  //

  // used to inform server which bucket's ll needs to be updated
  int update_bucket = 0;
  int benchmark_time = 5, full_iteration = 0;

  int total_sampled_tokens = 0, total_sampled_doc = 0;
  //
  uint64_t to_send_size;
  char* s3_local_vars_mem = s3_local_vars->serialize(to_send_size);
  model.reset(new LDAModel(s3_local_vars_mem));

  int cur = 0, num_runs = s3_local_vars->get_slice_size() / config.get_slice_size() + 1;

  while (1) {
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] running phase 1" << std::endl;
#endif
    // std::unique_ptr<LDAStatistics> local_vars;

    // If current LDAStatistics has been finished,
    // store the updated LDAStatistics back to S3
    // and need to inform server to update the corresponding ll
    // if (s3_local_vars->pop_partial_slice(local_vars) == -1) {
    if (cur >= num_runs) {

      update_bucket = cur_train_idx;
      total_sampled_doc += s3_local_vars->get_num_docs();

      s3_local_vars->store_new_stats(*model.get());

      help_upload_threads.push_back(std::make_unique<std::thread>(
            std::bind(&LDATaskS3::upload_wih_bucket_id_fn, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3),
            s3_local_vars, std::ref(upload_lock_indicators[cur_train_idx - start]), update_bucket)
      );

      // Get the next LDAStatistics from S3
      cur_train_idx += 1;
      if (cur_train_idx == end) {
        cur_train_idx = start;
        full_iteration += 1;

        // The early exit is added in case that
        // worker expires even if it hasn't stored the
        // updated LDAStatistics to S3
        auto elapsed_ms = get_time_ms() - start_time;
        float elapsed_sec = elapsed_ms / 1000.0;
        // 20 seconds before lambda times out
        if (elapsed_sec > (lambda_time_out - 30.0)) {
          // If there's a bucket that gets updated,
          // send an empty LDAUpdates with only the bucket id
          if (update_bucket != 0) {
            auto bucket_update = std::make_unique<LDAUpdates>(update_bucket);
            push_gradient(bucket_update.get());
          }

          std::cout << "--------------------------\n";
          std::cout << "Time to download from S3: " << time_download << std::endl;
          std::cout << "Time to send update to server: " << time_update << std::endl;
          std::cout << "Time to get model from server (whole): " << time_get_model << std::endl;
          std::cout << "Time to create model: " << time_create_model<< std::endl;
          std::cout << "Time to sample: " << time_sample << std::endl;
          std::cout << "--------------------------\n";
          std::cout << "Avg Time to download from S3: " << time_download / count << std::endl;
          std::cout << "Avg Time to send update to server: " << time_update / count << std::endl;
          std::cout << "Avg Time to get model from server (whole): " << time_get_model / count << std::endl;
          std::cout << "Avg Time to create model: " << time_create_model / count << std::endl;
          std::cout << "Avg Time to sample: " << time_sample / count  << std::endl;

          std::cout << "--------------------------\n";
          std::cout << "documents/sec: " << (double)total_sampled_doc / ((get_time_ms() - start_time) / 1000) << std::endl;
          std::cout << "tokens/sec: " << (double)total_sampled_tokens / ((get_time_ms() - start_time) / 1000) << std::endl;

          std::cout << "--------------------------\n";
          if (full_iteration == 0 ){
            std::cout << "sec/iteration: N/A yet \n";
          } else{
            std::cout << "sec/iteration: " << (double)full_iteration / ((get_time_ms() - start_time) / 1000) << std::endl;
          }

          std::cout << "successfully exit\n";
          break;
        }
      }

      if (end == start + 1) {
        s3_local_vars->reset_current();
        cur = 0;
        continue;
      }

      while (true) {
        std::this_thread::sleep_for (std::chrono::milliseconds(10));
        if (upload_lock_indicators[cur_train_idx - start] == -1){
          upload_lock_indicators[cur_train_idx - start] = 1;
          break;
        }
      }

      obj_id_str =
          std::to_string(hash_f(std::to_string(cur_train_idx).c_str())) +
          "-LDA";
      start_time_benchmark = get_time_ms();
      std::ostringstream* s3_obj =
          s3_client->s3_get_object_ptr(obj_id_str, config.get_s3_bucket());

      // upload_locks[cur_train_idx - train_range.first].lock();

      upload_lock_indicators[cur_train_idx - start] = -1;

      s3_local_vars.reset(new LDAStatistics(s3_obj->str().c_str()));
      s3_local_vars->set_slice_size(config.get_slice_size());
      time_download += (get_time_ms() - start_time_benchmark) / 1000.0;

      // s3_local_vars_mem = s3_local_vars->serialize(to_send_size);

      // char* s3_local_vars_mem = s3_local_vars->serialize(to_send_size);
      // model.reset(new LDAModel(s3_local_vars_mem));
      // delete s3_local_vars_mem;

      s3_local_vars_mem = s3_local_vars->serialize(to_send_size);
      model.reset(new LDAModel(s3_local_vars_mem));

      delete s3_obj;
      delete s3_local_vars_mem;

      num_runs = s3_local_vars->get_slice_size() / config.get_slice_size() + 1;
      cur = 0;

      continue;
    }

    cur += 1;

#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] phase 1 done. Getting the model"
              << std::endl;
    auto now = get_time_us();
#endif
    std::unique_ptr<LDAUpdates> gradient;

    // std::cout << "Getting new model\n";

    // we get the model subset with just the right amount of weights
    start_time_benchmark = get_time_ms();
    uint32_t to_receive_size, uncompressed_size;
    char* partial_model = psint->get_lda_model(to_receive_size, uncompressed_size);
    time_get_model += (get_time_ms() - start_time_benchmark) / 1000.0;

    // std::cout << "finish\n";
    start_time_benchmark = get_time_ms();
    // create_lda_model(*local_vars, update_bucket, partial_model, model, to_receive_size, uncompressed_size);

    // uint64_t to_send_size;
    // char* s3_local_vars_mem = s3_local_vars->serialize(to_send_size);
    // model.reset(new LDAModel(s3_local_vars_mem));

    // std::cout << "here?\n";
    model->update_model(partial_model, update_bucket, to_receive_size, uncompressed_size);
    // std::cout << "pass!\n";
    delete partial_model;

    time_create_model += (get_time_ms() - start_time_benchmark) / 1000.0;

    // if update_bucket is not 0,
    // then the bucket id has been sent to server in the above step
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

    // std::cout << "b4 sampling\n";

    try {
      start_time_benchmark = get_time_ms();
      gradient = model->sample_model(total_sampled_tokens);
      time_sample += (get_time_ms() - start_time_benchmark) / 1000.0;
    } catch (const std::runtime_error& e) {
      std::cout << "Error. " << e.what() << std::endl;
      exit(-1);
    }
    // } catch (...) {
    //   std::cout << "There was an error computing the gradient" << std::endl;
    //   exit(-1);
    // }
    // s3_local_vars->store_new_stats(*model.get());

    // std::cout << "finish sampling\n";

#ifdef DEBUG
    auto elapsed_us = get_time_us() - now;
    std::cout << "[WORKER] Gradient compute time (us): " << elapsed_us
              << " at time: " << get_time_us() << " version " << version
              << "\n";
#endif
    // gradient->setVersion(version++);

    try {
      start_time_benchmark = get_time_ms();
      push_gradient(gradient.get());
      time_update += (get_time_ms() - start_time_benchmark) / 1000.0;
    } catch (...) {
      std::cout << "[WORKER] "
                << "Worker task error doing put of gradient"
                << "\n";
      exit(-1);
    }

    // std::cout << "finish pushing update\n";

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
    int since_start_sec = (get_time_ms() - start_time) / 1000;
    if (since_start_sec > benchmark_time) {
      std::cout << "--------------------------\n";
      std::cout << "Time to download from S3: " << time_download << std::endl;
      std::cout << "Time to send update to server: " << time_update << std::endl;
      std::cout << "Time to get model from server (whole): " << time_get_model << std::endl;
      std::cout << "Time to create model: " << time_create_model<< std::endl;
      std::cout << "Time to sample: " << time_sample << std::endl;
      std::cout << "--------------------------\n";
      std::cout << "Avg Time to download from S3: " << time_download / count << std::endl;
      std::cout << "Avg Time to send update to server: " << time_update / count << std::endl;
      std::cout << "Avg Time to get model from server (whole): " << time_get_model / count << std::endl;
      std::cout << "Avg Time to create model: " << time_create_model / count << std::endl;
      std::cout << "Avg Time to sample: " << time_sample / count  << std::endl;

      std::cout << "--------------------------\n";
      std::cout << "documents/sec: " << (double)total_sampled_doc / ((get_time_ms() - start_time) / 1000) << std::endl;
      std::cout << "tokens/sec: " << (double)total_sampled_tokens / ((get_time_ms() - start_time) / 1000) << std::endl;

      std::cout << "--------------------------\n";
      if (full_iteration == 0 ){
        std::cout << "sec/iteration: N/A yet \n";
      } else{
        std::cout << "sec/iteration: " << (double)full_iteration / ((get_time_ms() - start_time) / 1000) << std::endl;
      }
      benchmark_time += 5;
    }
  }
  s3_shutdown_aws();
}

}  // namespace cirrus
