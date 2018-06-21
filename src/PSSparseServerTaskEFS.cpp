#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "Constants.h"
#include "Checksum.h"
#include <signal.h>
#include <NFSls.h>
#include <NFSFile.h>

#include <iostream>

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
std::map<std::string, uint64_t> path_to_size;

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
      std::cout << "entry.first: " << entry.first << std::endl;
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

        int grad_size = -1;
        std::cout << "Reading grad size.. #bytes: " << sizeof(grad_size) << std::endl;

        struct nfs_context *nfs = ls.nfs;
        struct nfsfh* file_handle;
        int ret2 = nfs_open(nfs, "/gradient3", O_CREAT | O_RDWR, &file_handle);

        char data[1000] = {0};
        ret2 = nfs_pread(nfs, file_handle, 0, sizeof(grad_size), (char*)&grad_size);
        std::cout << "len data: " << strlen(data) << std::endl;
        //ret2 = nfs_pread(nfs, file_handle, 0, size, data);
        nfs_close(nfs, file_handle);
        //NFSFile file("/netflix_dataset/" + entry.first);
        //file.read(off, (char*)&grad_size, sizeof(grad_size));

        std::cout << "data[0]: " << int(data[0]) << std::endl;
        std::cout << "grad size: " << grad_size << std::endl;
        if (grad_size <= 0) {
          // ignore this file
          continue;
        }

        char gradient_data[grad_size];
        std::cout << "Reading gradient " << std::endl;
        //file.read(off + sizeof(grad_size), gradient_data, grad_size);
        std::cout << "Read gradient" << std::endl;
      } else {
        continue; // otherwise ignore
      }
    }
    usleep(1000); // wait a ms if we haven't seen any changes
  }
}

void PSSparseServerTaskEFS::write_model() {

  uint64_t size = 0;
  std::shared_ptr<char> model = serialize_lr_model(*lr_model, &size);
  std::cout << "Writing model0 with size: " << size << std::endl;
  
  //NFSFile file("model0");
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
  ret = nfs_open(nfs, "/model0", O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error open");
  }
  ret = nfs_write(nfs, file_handle, 10000, model.get());
  if (ret <= 0) {
    throw std::runtime_error("Error nfs_write " + std::to_string(ret));
  }
  //ret = nfs_pwrite(nfs, file_handle, 0, 10000, model.get());
  //if (ret != 0) {
  //  throw std::runtime_error("Error pwrite " + std::to_string(ret));
  //}
  nfs_close(nfs, file_handle);
    
  nfs_destroy_context(nfs);
  free(server); 
  free(path); 
  free(url); 
}

void PSSparseServerTaskEFS::run(const Configuration& config) {
  std::cout
    << "PS task initializing model"
    << std::endl;
  lr_model.reset(new SparseLRModel(model_size));
  lr_model->randomize();

  task_config = config;

  write_model(); // write version 0 of the model to efs


  while (1) {
    // we sit in a loop checking gradients and updating model
    auto grad = check_gradient();
    apply_to_model(grad);
  }

}

} // namespace cirrus
