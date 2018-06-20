#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "Constants.h"
#include "Checksum.h"
#include <signal.h>
#include <NFSls.h>

#undef DEBUG

#define MAX_CONNECTIONS (nworkers * 2 + 1) // (2 x # workers + 1)
#define THREAD_MSG_BUFFER_SIZE 5000000

namespace cirrus {

PSSparseServerTaskEFS::PSSparseServerTaskEFS(
    uint64_t model_size,
    uint64_t batch_size, uint64_t samples_per_batch,
    uint64_t features_per_sample, uint64_t nworkers,
    uint64_t worker_id, const std::string& ps_ip,
    uint64_t ps_port) :
  MLTask(model_size,
      batch_size, samples_per_batch, features_per_sample,
      nworkers, worker_id, ps_ip, ps_port) {
    std::cout << "PSSparseServerTaskEFS is built" << std::endl;
}

std::shared_ptr<char> PSSparseServerTaskEFS::serialize_lr_model(
    const SparseLRModel& lr_model, uint64_t* model_size) const {
  *model_size = lr_model.getSerializedSize();
  auto d = std::shared_ptr<char>(
      new char[*model_size], std::default_delete<char[]>());
  lr_model.serializeTo(d.get());
  return d;
}

void PSSparseServerTaskEFS::apply_to_model(LRSparseGradient& grad) {

}

// for every gradient file we keep track of the offset
// we are reading at
std::map<std::string, uint64_t> path_to_offset;

LRSparseGradient PSSparseServerTaskEFS::check_gradient() {
  NFSls ls("/efs_experiment/");
  std::vector<std::pair<std::string, uint64_t>> result = ls.do_ls();

  for (const auto& entry : result) {
    std::string path = entry.first;
    // if entry starts with "gradient"
    // we check if we have seen this before
    if (path.compare(0, std::string("gradient").size(), "gradient") == 0) {
      if (path_to_offset.find(path) != path_to_offset.end()) {
        uint64_t off = path_to_offset[path];
        std::cout << "Reading from off: " << off << std::endl;
      } else {
        path_to_offset[path] = 0; // we start reading from the beginning
      }
    } else {
      continue; // otherwise ignore
    }
  }
}

void PSSparseServerTaskEFS::run(const Configuration& config) {
  std::cout
    << "PS task initializing model"
    << std::endl;
  lr_model.reset(new SparseLRModel(model_size));
  lr_model->randomize();

  task_config = config;


  while (1) {
    // we sit in a loop checking gradients and updating model
    auto grad = check_gradient();
    apply_to_model(grad);
  }

}

} // namespace cirrus
