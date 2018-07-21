#include <Tasks.h>

#include <InputReader.h>

namespace cirrus {
void SoftmaxTask::push_gradient(SoftmaxGradient* smg) {
#ifdef DEBUG
  auto before_push_us = get_time_us();
  std::cout << "Publishing gradients" << std::endl;
#endif
  psint->send_sm_gradient(*smg);
#ifdef DEBUG
  std::cout << "Published gradients!" << std::endl;
  auto elapsed_push_us = get_time_us() - before_push_us;
  static uint64_t before = 0;
  if (before == 0)
    before = get_time_us();
  auto now = get_time_us();
  std::cout << "[WORKER] "
      << "Worker task published gradient"
      << " with version: " << smg->getVersion()
      << " at time (us): " << get_time_us()
      << " took(us): " << elapsed_push_us
      << " bw(MB/s): " << std::fixed <<
         (1.0 * smg->getSerializedSize() / elapsed_push_us / 1024 / 1024 * 1000 * 1000)
      << " since last(us): " << (now - before)
      << "\n";
  before = now;
#endif
}

void SoftmaxTask::run(const Configuration& config, int worker) {
  std::cout << "Starting SoftmaxTask"
    << std::endl;
  this->config = config;
  InputReader input;
  Dataset dataset_train = input.read_input_csv("test_data/train_mnist.csv", ",", 10, 50000, 1000, true);

  psint = new PSSparseServerInterface(ps_ip, ps_port);
  
  wait_for_start(worker, nworkers);

  // Create iterator that goes from 0 to num_s3_batches

  std::cout << "[WORKER] starting loop" << std::endl;

  uint64_t version = 1;

  bool printed_rate = false;
  int count = 0;
  auto start_time = get_time_ms();
  while (1) {
    // get data, labels and model
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] running phase 1" << std::endl;
#endif
    Dataset dataset = dataset_train.random_sample(20);
#ifdef DEBUG
    std::cout << get_time_us() << " [WORKER] phase 1 done. Getting the model" << std::endl;
    //dataset->check();
    //dataset->print_info();
    auto now = get_time_us();
#endif
    // compute mini batch gradient
    std::unique_ptr<ModelGradient> gradient;

    // we get the model subset with just the right amount of weights
    SoftmaxModel model = *(psint->get_sm_full_model());
    model.d = 784;
    model.nclasses = 10;
#ifdef DEBUG
    std::cout << "get model elapsed(us): " << get_time_us() - now << std::endl;
    std::cout << "Checking model" << std::endl;
    //model.check();
    std::cout << "Computing gradient" << "\n";
    now = get_time_us();
#endif
    dataset.print_info();
    try {
      gradient = model.minibatch_grad(dataset.get_samples(), (float*) dataset.get_labels().get(), 20, 0.0001);
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
      SoftmaxGradient* smg = dynamic_cast<SoftmaxGradient*>(gradient.get());
      push_gradient(smg);
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
}
