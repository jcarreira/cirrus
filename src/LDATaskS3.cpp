#include <Tasks.h>

#include "PSSparseServerInterface.h"
#include "S3Client.h"
#include "S3SparseIterator.h"
#include "Serializers.h"
#include "Utils.h"

#include <pthread.h>
#include <thread>

#undef DEBUG

namespace cirrus {

void LDATaskS3::push_gradient(char* gradient_mem,
                              int total_sampled_tokens,
                              int total_sampled_docs,
                              uint32_t to_send_size) {
#ifdef DEBUG
  auto before_push_us = get_time_us();
  std::cout << "Publishing gradients" << std::endl;
#endif
  psint->send_lda_update(gradient_mem, total_sampled_tokens, total_sampled_docs,
                         to_send_size + sizeof(int) * 2);
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

void LDATaskS3::upload_wih_bucket_id_fn(char* mem_to_send,
                                        uint64_t to_send_size,
                                        int& upload_lock,
                                        int bucket_id) {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (upload_lock == -1) {
      upload_lock = 1;
      break;
    }
  }

  std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();
  std::string obj_id_str =
      std::to_string(hash_f(std::to_string(bucket_id).c_str())) + "-LDA";
  s3_client->s3_put_object(obj_id_str, this->config.get_s3_bucket(),
                           std::string(mem_to_send, to_send_size));

  upload_lock = -1;
  delete mem_to_send;
}

int LDATaskS3::load_serialized_indices(char* mem_begin) {
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

  int global_vocab_dim = load_value<int32_t>(mem);
  return global_vocab_dim;
}

void LDATaskS3::print_status() const {
  std::cout << "--------------------------\n";
  std::cout << "Time to download from S3: " << time_download << std::endl;
  std::cout << "Time to send update to server: " << time_update << std::endl;
  std::cout << "Time to get model from server (whole): " << time_get_model
            << std::endl;
  std::cout << "Time to create model: " << time_create_model << std::endl;
  std::cout << "Time to sample: " << time_sample << std::endl;
  std::cout << "--------------------------\n";
  std::cout << "Avg Time to download from S3: " << time_download / count
            << std::endl;
  std::cout << "Avg Time to send update to server: " << time_update / count
            << std::endl;
  std::cout << "Avg Time to get model from server (whole): "
            << time_get_model / count << std::endl;
  std::cout << "Avg Time to create model: " << time_create_model / count
            << std::endl;
  std::cout << "Avg Time to sample: " << time_sample / count << std::endl;

  std::cout << "--------------------------\n";
  std::cout << "documents/sec: "
            << (double) total_sampled_docs /
                   ((get_time_ms() - start_time) / 1000)
            << std::endl;
  std::cout << "tokens/sec: "
            << (double) total_sampled_tokens /
                   ((get_time_ms() - start_time) / 1000)
            << std::endl;

  std::cout << "--------------------------\n";
  if (full_iteration == 0) {
    std::cout << "sec/iteration: N/A yet \n";
  } else {
    std::cout << "sec/iteration: "
              << (double) full_iteration / ((get_time_ms() - start_time) / 1000)
              << std::endl;
  }
}

void LDATaskS3::run(const Configuration& config, int worker_id, int test_iters) {
  double lambda_time_out = 900.0;  // 15 min currently

  std::cout << "Starting LDATaskS3" << std::endl;
  this->config = config;

  psint = new PSSparseServerInterface(ps_ip, ps_port);
  psint->connect();

  std::cout << "[WORKER] " << "num s3 batches: "
            << config.get_limit_samples() / config.get_s3_file_size()
            << std::endl;

  // pre-assign the documents for workers
  auto train_range = config.get_train_range();
  int range_per_worker =
      (train_range.second - train_range.first + 1) / nworkers;
  int start = worker_id * range_per_worker + 1,
      end = (worker_id + 1) * range_per_worker + 1;
  if (end > train_range.second) {
    end = train_range.second;
  }
  int cur_train_idx = start;

  bool printed_rate = false;
  start_time = get_time_ms();

  int update_bucket = 0;
  int benchmark_time = 5, full_iteration = 0;
  uint32_t to_receive_size, uncompressed_size;
  uint64_t version = 1;
  upload_lock_indicators.resize(end - start, -1);

  // Load the first LDAStatistics from S3
  auto start_time_benchmark = get_time_ms();
  // s3_initialize_aws();
  std::shared_ptr<S3Client> s3_client = std::make_shared<S3Client>();
  std::string obj_id_str =
      std::to_string(hash_f(std::to_string(cur_train_idx).c_str())) + "-LDA";
  std::ostringstream* s3_obj =
      s3_client->s3_get_object_ptr(obj_id_str, config.get_s3_bucket());
  time_download += (get_time_ms() - start_time_benchmark) / 1000.0;

  // create a LDAModel object with local statistics from S3
  std::unique_ptr<LDAModel> model;
  uint64_t to_send_size;
  model.reset(new LDAModel(s3_obj->str().c_str()));
  delete s3_obj;

  // load the pre-cached indices from server
  char* slice_indices_mem = psint->get_slices_indices(cur_train_idx);
  int global_vocab_dim = load_serialized_indices(slice_indices_mem);
  delete slice_indices_mem;

  // num_runs: the number of iters to run for one S3 object (LDAStatistics)
  int cur = 0, num_runs = global_vocab_dim / config.get_slice_size();
  std::cout << "Initalized global vocab dim: " << global_vocab_dim << std::endl;
  std::cout << "Initalized num_runs: " << num_runs << std::endl;
  std::cout << "[WORKER] starting loop" << std::endl;

  while (1) {
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] running phase 1" << std::endl;
#endif
    // only True if current LDAStatistics has been sampled with
    // more than (num_runs) number of word slices
    if (cur % num_runs == 0 && cur != 0) {
      // update_bucket = cur_train_idx;
      total_sampled_docs += model->get_ndt_size();

      // only send to S3 if the next LDAStatistics is not the current one
      if (end != start + 1) {
        uint64_t size_temp;
        char* mem_to_send = model->serialize_to_S3(size_temp);

        help_upload_threads.push_back(std::make_unique<std::thread>(
            std::bind(&LDATaskS3::upload_wih_bucket_id_fn, this,
                      std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3, std::placeholders::_4),
            mem_to_send, size_temp,
            std::ref(upload_lock_indicators[cur_train_idx - start]),
            cur_train_idx));
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
        float est_time_one_iter =
            ((get_time_ms() - start_time) / 1000.) / full_iteration;

        if (elapsed_sec > (lambda_time_out - est_time_one_iter - 10.)) {
          uint64_t size_temp;
          char* mem_to_send = model->serialize_to_S3(size_temp);
          upload_wih_bucket_id_fn(mem_to_send, size_temp,
                                  upload_lock_indicators[0], cur_train_idx);

          psint->send_time_dist(time_sample,
                                time_download + time_update + time_get_model);
          print_status();
          std::cout << "successfully exit\n";
          break;
        }
      }

      // if there's only one LDAStatistics assigned to this worker,
      // simply reset (cur) to avoid uploading & downloading
      if (end == start + 1) {
        continue;
      }

      std::cout << "here?\n";

      // load the next LDAStatistics from S3
      // Waits if the uploading to the next LDAStatistics has not finished
      while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (upload_lock_indicators[cur_train_idx - start] == -1) {
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
      time_download += (get_time_ms() - start_time_benchmark) / 1000.0;

      // create the LDAModel
      model.reset(new LDAModel(s3_obj->str().c_str()));

      delete s3_obj;

      // load the pre-cached token indices from server
      slice_indices_mem = psint->get_slices_indices(cur_train_idx);
      load_serialized_indices(slice_indices_mem);
      delete slice_indices_mem;

      continue;
    }

    cur += 1;

    start_time_benchmark = get_time_ms();
    // load one word slice from server
    char* partial_model =
        psint->get_lda_model(to_receive_size, uncompressed_size);
    time_get_model += (get_time_ms() - start_time_benchmark) / 1000.0;

    start_time_benchmark = get_time_ms();
    // update current LDAModel
    model->update_model(partial_model, to_receive_size, uncompressed_size,
                        psint->slice_id);
    time_create_model += (get_time_ms() - start_time_benchmark) / 1000.0;

    // sampling phase
    char* gradient_mem;
    uint32_t gradient_size;
    start_time_benchmark = get_time_ms();
    gradient_mem = model->sample_model(
        total_sampled_tokens, slice_indices[psint->slice_id], gradient_size);
    time_sample += (get_time_ms() - start_time_benchmark) / 1000.0;

    // push the update to server
    try {
      start_time_benchmark = get_time_ms();
      push_gradient(gradient_mem, total_sampled_tokens, total_sampled_docs,
                    gradient_size);

      total_sampled_docs = 0;

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
      print_status();
      benchmark_time += 5;
    }

    // loop breaks only if the number of iters has been met
    if (test_iters > 0 && full_iteration > test_iters) {
      exit(0);
    }
  }
}

}  // namespace cirrus
