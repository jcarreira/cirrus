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

void LDATaskS3::push_gradient(char* gradient_mem, int total_sampled_tokens, uint32_t to_send_size) {
#ifdef DEBUG
  auto before_push_us = get_time_us();
  std::cout << "Publishing gradients" << std::endl;
#endif
  psint->send_lda_update(gradient_mem, total_sampled_tokens, to_send_size);
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

void LDATaskS3::upload_wih_bucket_id_fn(std::shared_ptr<LDAStatistics> to_save,
                             int& upload_lock,
                             int bucket_id){

  while (true) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
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

void LDATaskS3::load_serialized_indices(char* mem_begin){

  const char* mem = mem_begin;

  int num_slices = load_value<int32_t>(mem);
  slice_indices.clear();
  slice_indices.reserve(num_slices);
  for (int i = 0; i < num_slices; ++i) {
    int len = load_value<int32_t>(mem);
    std::vector<int> slice_i;
    slice_i.reserve(len);
    for (int j = 0; j < len; ++j) {
      int temp = load_value<int32_t>(mem);
      slice_i.push_back(temp);
    }
    slice_indices.push_back(slice_i);
  }
}


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
  int cur_train_idx = start;

  bool printed_rate = false;
  int count = 0;
  auto start_time = get_time_ms();
  std::shared_ptr<LDAStatistics> s3_local_vars;

  int update_bucket = 0;
  int benchmark_time = 5, full_iteration = 0;
  uint32_t to_receive_size, uncompressed_size;
  int total_sampled_tokens = 0, total_sampled_doc = 0;
  uint64_t version = 1;
  upload_lock_indicators.resize(end - start, -1);

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

  // create a LDAModel object with local statistics from S3
  std::unique_ptr<LDAModel> model;
  uint64_t to_send_size;
  char* s3_local_vars_mem = s3_local_vars->serialize(to_send_size);
  model.reset(new LDAModel(s3_local_vars_mem));
  delete s3_local_vars_mem;

  // load the pre-cached indices from server
  char* slice_indices_mem = psint->get_slices_indices(cur_train_idx);
  load_serialized_indices(slice_indices_mem);
  delete slice_indices_mem;

  int cur = 0, num_runs = s3_local_vars->get_slice_size() / config.get_slice_size() + 1;
  std::cout << "[WORKER] starting loop" << std::endl;

  while (1) {
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] running phase 1" << std::endl;
#endif
    // only True if current LDAStatistics has been sampled with
    // more than (num_runs) number of word slices
    if (cur >= num_runs) {

      update_bucket = cur_train_idx;
      total_sampled_doc += s3_local_vars->get_num_docs();

      // store the updated doc-topics statistics to the LDAStatistics
      // which will be sent to S3
      s3_local_vars->store_new_stats(*model.get());

      // only send to S3 if the next LDAStatistics is not the current one
      if (end != start + 1) {
        help_upload_threads.push_back(std::make_unique<std::thread>(
              std::bind(&LDATaskS3::upload_wih_bucket_id_fn, this,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3),
              s3_local_vars, std::ref(upload_lock_indicators[update_bucket - start]), update_bucket)
        );
      }

      // compute the loglikelihood for current LDAStatistics and
      // send to server
      double ll_to_send = model->compute_ll_ndt();
      psint->update_ll_ndt(cur_train_idx, ll_to_send);

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

        // estimate average time to sample for one iteration
        float est_time_one_iter = ((get_time_ms() - start_time) / 1000.) / full_iteration;

        if (elapsed_sec > (lambda_time_out - est_time_one_iter * 2)) {

          // true only if the current LDAStatistics has not been
          // pushed to S3 yet
          if (end == start + 1) {
            upload_wih_bucket_id_fn(s3_local_vars, upload_lock_indicators[cur_train_idx - start], update_bucket);
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

      // if there's only one LDAStatistics assigned to this worker,
      // simply reset (cur) to avoid uploading & downloading
      if (end == start + 1) {
        cur = 0;
        continue;
      }

      // load the next LDAStatistics from S3
      while (true) {
        std::this_thread::sleep_for (std::chrono::milliseconds(1));
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

      upload_lock_indicators[cur_train_idx - start] = -1;

      s3_local_vars.reset(new LDAStatistics(s3_obj->str().c_str()));
      s3_local_vars->set_slice_size(config.get_slice_size());
      time_download += (get_time_ms() - start_time_benchmark) / 1000.0;

      // create the LDAModel
      s3_local_vars_mem = s3_local_vars->serialize(to_send_size);
      model.reset(new LDAModel(s3_local_vars_mem));

      delete s3_obj;
      delete s3_local_vars_mem;

      // load the pre-cached token indices from server
      slice_indices_mem = psint->get_slices_indices(cur_train_idx - start);
      load_serialized_indices(slice_indices_mem);
      delete slice_indices_mem;

      num_runs = s3_local_vars->get_slice_size() / config.get_slice_size() + 1;
      cur = 0;

      continue;
    }

    cur += 1;

    start_time_benchmark = get_time_ms();
    // load one word slice from server
    char* partial_model = psint->get_lda_model(cur_train_idx, to_receive_size, uncompressed_size);
    time_get_model += (get_time_ms() - start_time_benchmark) / 1000.0;

    start_time_benchmark = get_time_ms();
    // update current LDAModel
    model->update_model(partial_model, to_receive_size, uncompressed_size, psint->slice_id);
    time_create_model += (get_time_ms() - start_time_benchmark) / 1000.0;

    // sampling phase
    char* gradient_mem;
    uint32_t gradient_size;
    start_time_benchmark = get_time_ms();
    gradient_mem = model->sample_model(total_sampled_tokens, slice_indices[psint->slice_id], gradient_size);
    time_sample += (get_time_ms() - start_time_benchmark) / 1000.0;

    // push the update to server
    try {
      start_time_benchmark = get_time_ms();
      push_gradient(gradient_mem, total_sampled_tokens, gradient_size);
      time_update += (get_time_ms() - start_time_benchmark) / 1000.0;
    } catch (...) {
      std::cout << "[WORKER] "
                << "Worker task error doing put of gradient"
                << "\n";
      exit(-1);
    }

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
