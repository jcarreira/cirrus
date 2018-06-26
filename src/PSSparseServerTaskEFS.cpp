#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "Constants.h"
#include "Checksum.h"
#include <signal.h>
#include <NFSls.h>
#include <NFSFile.h>

#include <iostream>
#include <algorithm>

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
  lr_model->sgd_update(task_config.get_learning_rate(), &grad);
}

// for every gradient file we keep track of the offset
// we are reading at
std::map<std::string, uint64_t> path_to_offset;
std::map<std::string, uint64_t> path_to_size;

/** Each worker works on a gradient gradient<worker id>
  * Repeatedly search for file with name gradient*
  * Jump to the offset of the next gradient in gradient<worker id>
  * Read size of the gradient (32 bits)
  * Read gradient
  *
  */
LRSparseGradient PSSparseServerTaskEFS::check_gradient() {
  NFSls ls("/");

  while (1) {
    std::cout << "LOOP" << std::endl;
    std::vector<std::pair<std::string, uint64_t>> result = ls.do_ls();
    std::cout << "LS returned " << result.size() << " entries" << std::endl;

    for (const auto& entry : result) {
      std::string path = entry.first;
      // if entry starts with "gradient"
      // we check if we have seen this file before
      std::cout << "checking file: " << path << std::endl;
      if (path.compare(0, std::string("gradient").size(), "gradient") == 0) {
        uint64_t off = 0;
        uint64_t size = 0;
        size = path_to_size[path] = entry.second;
        if (path_to_offset.find(path) != path_to_offset.end()) {
          off = path_to_offset[path];
        } else {
          off = path_to_offset[path] = 0; // we start reading from the beginning
        }

        std::cout << "Reading from off: " << off <<  " size " << size << std::endl;

        int32_t grad_size = -1;

        NFSFile file(ls.nfs, "/" + path);
        int ret = file.read(off, (char*)&grad_size, sizeof(grad_size));
        std::cout << "grad size: " << grad_size << std::endl;
        if (ret != sizeof(grad_size)) {
          std::cout << "Error reading gradient size" << std::endl;
          continue;
        }
        std::cout << "grad size: " << grad_size << std::endl;
        if (grad_size > 10 * 1024 * 1024) {
          throw std::runtime_error("Gradient is too large");
        }

        std::shared_ptr<char[]> grad_data(new char[grad_size]);
        ret = file.read(off, grad_data.get(), grad_size);
        if (ret != grad_size) {
          std::cout << "Error reading gradient" << std::endl;
          continue;
        }

        LRSparseGradient gradient(0);
        gradient.loadSerialized(grad_data.get());
        return gradient;
      } else {
        continue; // otherwise ignore
      }
    }
    usleep(1000); // wait a ms if we haven't seen any changes
  }
}

int write_(
  struct nfs_context *nfs,
  struct nfsfh* file_handle,
  uint64_t off,
  uint64_t write_size,
  const char* data) {

  uint64_t to_write = write_size;
  uint64_t write_block_size = 1000000;
  uint64_t written = 0;

  while (to_write) {
    uint64_t to_write_now = std::min(to_write, write_block_size);
    int ret = nfs_pwrite(nfs, file_handle, written, to_write_now, data);
    if (ret <= 0) {
      throw std::runtime_error("Error in write_ " + std::to_string(ret));
    }
    
    written += ret;
    to_write -= ret;

    std::cout << "nfs_pwrite"
     << " ret: " << ret
     << " to_write: " << to_write
     << " written: " << written
     << std::endl;
  }

  return written;
}

void PSSparseServerTaskEFS::write_model(uint64_t version) {

  uint64_t size = 0;
  std::shared_ptr<char> model = serialize_lr_model(*lr_model, &size);
  std::string filename = "model" + std::to_string(version);
  std::cout << "Writing " << filename
    << " with size: " << size
    << std::endl;
  
  //NFSFile file("/" + filename);
  //file.write(0, model.get(), size);

  struct nfs_context *nfs = nullptr;
  struct nfs_url *url = nullptr;
  nfs = nfs_init_context();
  if (nfs == NULL) {
    throw std::runtime_error("Error initing nfs");
  }
  
  url = nfs_parse_url_dir(nfs, "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
  if (url == NULL) {
    throw std::runtime_error("Error parsing nfs url");
  }

  char *server = url->server;
  char* path = url->path;

  std::cout << "Mounting nfs" << std::endl;
  int ret = nfs_mount(nfs, server, path);
  if (ret != 0) {
    throw std::runtime_error("Error mounting nfs");
  }

  struct nfsfh* file_handle;
  ret = nfs_open(nfs, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error open");
  }

  uint64_t write_size = size;
  std::cout << "Writing model with size: " << size
    << " real write size: " << write_size << std::endl;

  ret = write_(nfs, file_handle, 0, write_size, model.get());
  //ret = nfs_pwrite(nfs, file_handle, 0, write_size, model.get());
  //ret = nfs_write(nfs, file_handle, write_size, model.get());
  //ret = nfs_write(nfs, file_handle, size, model.get());
  if (ret <= 0) {
    throw std::runtime_error("Error nfs_pwrite " + std::to_string(ret));
  }
  //ret = nfs_pwrite(nfs, file_handle, 0, 10000, model.get());
  //if (ret != 0) {
  //  throw std::runtime_error("Error pwrite " + std::to_string(ret));
  //}
  nfs_close(nfs, file_handle);
  std::cout << "Wrote and closed file" << std::endl;
    
  nfs_destroy_context(nfs);
  free(server); 
  free(path); 
  free(url); 
}

void PSSparseServerTaskEFS::run(const Configuration& config) {
  std::cout
    << "PS task initializing model"
    << " size: " << model_size
    << std::endl;

  lr_model.reset(new SparseLRModel(model_size));
  lr_model->randomize();

  task_config = config;

  uint64_t model_version = 0;
  auto before = get_time_us();
  write_model(model_version++); // write version 0 of the model to efs
  auto elapsed_time_us = get_time_us() - before;
  std::cout << "write_model bw MB/s: "
    << (model_size * 4 / 1024.0 / 1024 / elapsed_time_us * 1000 * 1000)
    << std::endl;

  while (1) {
    // we sit in a loop checking gradients and updating model
    auto grad = check_gradient();
    apply_to_model(grad);
    write_model(model_version++);
  }

}

} // namespace cirrus
