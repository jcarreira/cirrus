#include <unistd.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include <InputReader.h>
#include <SparseLRModel.h>
#include <S3SparseIterator.h>

#include "../config.h"
#include "../Utils.h"

//typedef float FEATURE_TYPE;
const std::string INPUT_PATH = "../../data/rcv1/train.vw";

void print_info(const auto& samples) {
  std::cout << "Number of samples: " << samples.size() << std::endl;
  std::cout << "Number of cols: " << samples[0].size() << std::endl;
}

void check_error(auto model, auto dataset) {
  auto ret = model->calc_loss(dataset, 0);
  auto total_loss = ret.first;
  auto avg_loss = 1.0 * total_loss / dataset.num_samples();
  auto acc = ret.second;
  std::cout
    << "time: " << get_time_us()
    << " total/avg loss: " << total_loss << "/" << avg_loss
    << " accuracy: " << acc
    << std::endl;
}

std::mutex model_lock;
std::mutex s3_lock;;
std::unique_ptr<SparseLRModel> model;
double epsilon = 0.00001;
double learning_rate = 0.00000001;

void learning_function(const SparseDataset& dataset) {
  for (uint64_t i = 0; 1; ++i) {
    //std::cout << "iter" << std::endl;
    SparseDataset ds = dataset.random_sample(20);

    auto gradient = model->minibatch_grad(
        ds, epsilon);

    model_lock.lock();
    model->sgd_update(learning_rate, gradient.get());
    model_lock.unlock();
  }
}

  
Configuration config;
S3SparseIterator* s3_iter;
void learning_function_from_s3(const SparseDataset& dataset) {
#if 0
  S3SparseIterator s3_iter(0, 10, config,
      config.get_s3_size(),
      config.get_minibatch_size());
#endif

  for (uint64_t i = 0; 1; ++i) {
    s3_lock.lock();
    const void* data = s3_iter->get_next_fast();
    s3_lock.unlock();
    SparseDataset ds(reinterpret_cast<const char*>(data),
        config.get_minibatch_size()); // construct dataset with data from s3

    auto gradient = model->minibatch_grad(dataset, epsilon);

    model_lock.lock();
    model->sgd_update(learning_rate, gradient.get());
    model_lock.unlock();
  }
}

/**
  * Code not working
  */
int main() {
  InputReader input;
  SparseDataset dataset = input.read_input_rcv1_sparse(
      INPUT_PATH,
      " ",
      100000,
      true); // normalize=true
  dataset.check();
  dataset.print_info();

  //config.read("criteo_aws_lambdas_s3.cfg");
  //s3_iter = new S3SparseIterator(0, 10, config,
  //    config.get_s3_size(),
  //    config.get_minibatch_size());

  uint64_t model_size = (1 << 19);
  model.reset(new SparseLRModel(model_size));

  uint64_t num_threads = 1;
  std::vector<std::shared_ptr<std::thread>> threads;
  for (uint64_t i = 0; i < num_threads; ++i) {
    threads.push_back(std::make_shared<std::thread>(
          learning_function, dataset));
  }

  while (1) {
    usleep(100000); // 100ms
    model_lock.lock();
    check_error(model.get(), dataset);
    model_lock.unlock();
  }
  return 0;
}

