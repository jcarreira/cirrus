#include <Tasks.h>

#include "Serializers.h"
#include "Utils.h"
#include "Constants.h"
#include "Checksum.h"
#include <signal.h>

#ifdef USE_EFS
#include <NFSls.h>
#include <NFSFile.h>
#endif

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
  std::cout << "Applying gradient to model" << std::endl;
  lr_model->sgd_update(task_config.get_learning_rate(), &grad);
  //std::cout << "print gradient" << std::endl;
  //grad.print();
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
  NFSls ls(nfs_, "/");

  while (1) {
    std::cout << "LOOP" << std::endl;

    auto t1 = get_time_us();
    std::vector<std::pair<std::string, uint64_t>> result = ls.do_ls();
    auto elapsed1 = get_time_us() - t1;
    std::cout << "do_ls took (us): " << elapsed1 << std::endl;
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

        NFSFile file(nfs_, "/" + path);
        int ret = file.read(off, (char*)&grad_size, sizeof(int32_t));
        std::cout << "grad size: " << grad_size << std::endl;
        if (ret != sizeof(grad_size)) {
          std::cout << "Error reading gradient size" << std::endl;
          continue;
        }
        if (grad_size > 10 * 1024 * 1024) {
          throw std::runtime_error("Gradient is too large");
        }

        std::shared_ptr<char[]> grad_data(new char[grad_size]);
        ret = file.read(off + sizeof(int32_t), grad_data.get(), grad_size);
        if (ret != grad_size) {
          std::cout << "Error reading gradient" << std::endl;
          continue;
        }
          
        path_to_offset[path] += grad_size + sizeof(int32_t);

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

LRSparseGradient PSSparseServerTaskEFS::check_gradient2() {
  while (1) {
    std::cout << "LOOP" << std::endl;

    auto time1 = get_time_us();
    std::vector<std::pair<std::string, uint64_t>> result;
    // try to open all files of the form "gradientX"
    for (int i = 0; i < 10; ++i) {
      std::string filename = "gradient" + std::to_string(i);
      struct nfsfh* file_handle;
      int ret = nfs_open(nfs_, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
      if (ret != 0) {
        continue;
      }
      nfs_close(nfs_, file_handle);

      struct nfs_stat_64 st; 
      char dir_file_path[1024];
      sprintf(dir_file_path, "%s/%s", path_, filename.c_str());
      //std::cout << "efs path: " << dir_file_path << " filename: " << filename << std::endl;
      ret = nfs_stat64(nfs_, dir_file_path, &st);
      if (ret != 0) {
        fprintf(stderr, "Failed to stat(%s) %s\n", dir_file_path, nfs_get_error(nfs_));
        continue;
      }
      result.push_back(std::make_pair(filename, st.nfs_size));
    }
    auto time2 = get_time_us();

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

        NFSFile file(nfs_, "/" + path);
        int ret = file.read(off, (char*)&grad_size, sizeof(int32_t));
        std::cout << "grad size: " << grad_size << std::endl;
        if (ret != sizeof(grad_size)) {
          std::cout << "Error reading gradient size" << std::endl;
          continue;
        }
        if (grad_size > 10 * 1024 * 1024) {
          throw std::runtime_error("Gradient is too large");
        }

        std::shared_ptr<char[]> grad_data(new char[grad_size]);
        ret = file.read(off + sizeof(int32_t), grad_data.get(), grad_size);
        if (ret != grad_size) {
          std::cout << "Error reading gradient" << std::endl;
          continue;
        }
          
        path_to_offset[path] += grad_size + sizeof(int32_t);

        LRSparseGradient gradient(0);
        gradient.loadSerialized(grad_data.get());
        auto time3 = get_time_us();
        std::cout << "elapsed1: " << (time2 - time1)
                  << "elapsed2: " << (time3 - time2)
                  << std::endl;
        return gradient;
      } else {
        continue; // otherwise ignore
      }
    }
    //usleep(1000); // wait a ms if we haven't seen any changes
  }
}

#if 1
int nfs_write_wrapper(
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
    int ret = nfs_pwrite(nfs, file_handle, written, to_write_now, data + written);
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
#endif

void PSSparseServerTaskEFS::write_model2(uint64_t version) {

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
  
  url = nfs_parse_url_dir(nfs,
      "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
  if (url == NULL) {
    throw std::runtime_error("Error parsing nfs url");
  }

  char *server = url->server;
  char* path = url->path;

  std::cout << "Mounting nfs" << std::endl;
  int ret = nfs_mount(nfs, server, path);
  if (ret != 0) {
    throw std::runtime_error("Error mounting nfs ret: " + std::to_string(ret));
  }

  struct nfsfh* file_handle;
  ret = nfs_open(nfs, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error open");
  }

  uint32_t write_size = size;
  std::cout << "Writing model with size: " << size
    << " real write size: " << write_size << std::endl;

  ret = nfs_write_wrapper(
      nfs, file_handle, sizeof(uint32_t), write_size, model.get());
  if (ret <= 0) {
    throw std::runtime_error("Error nfs_pwrite " + std::to_string(ret));
  }
  nfs_close(nfs, file_handle);
  std::cout << "Wrote and closed file" << std::endl;
    
  nfs_destroy_context(nfs);
  free(server); 
  free(path); 
  free(url); 
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

#if 0
  static struct nfs_context *nfs = nullptr;
  static struct nfs_url *url = nullptr;
  static bool done = false;
  
  if (done == false) {
      done = true;
      nfs = nfs_init_context();
      if (nfs == NULL) {
          throw std::runtime_error("Error initing nfs");
      }

      url = nfs_parse_url_dir(nfs,
              "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
      if (url == NULL) {
          throw std::runtime_error("Error parsing nfs url");
      }

      char *server = url->server;
      char* path = url->path;

      std::cout << "Mounting nfs" << std::endl;
      int ret = nfs_mount(nfs, server, path);
      if (ret != 0) {
          throw std::runtime_error("Error mounting nfs ret: " + std::to_string(ret));
      }
  }
#endif

  struct nfsfh* file_handle;
  int ret = nfs_open(nfs_, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error open");
  }

  uint32_t write_size = size;
  std::cout << "Writing model with size: " << size
    << " real write size: " << write_size << std::endl;

  ret = nfs_write_wrapper(
      nfs_, file_handle, sizeof(uint32_t), write_size, model.get());
  if (ret <= 0) {
    throw std::runtime_error("Error nfs_pwrite " + std::to_string(ret));
  }
  nfs_close(nfs_, file_handle);
  std::cout << "Wrote and closed file" << std::endl;
    
  //nfs_destroy_context(nfs);
  //free(server); 
  //free(path); 
  //free(url); 
}

void PSSparseServerTaskEFS::write_model3(uint64_t version) {
  std::string filename = "model" + std::to_string(version);
  std::cout << "Writing " << filename
    << std::endl;
  
  static struct nfs_context *nfs = nullptr;
  static struct nfs_url *url = nullptr;
  static bool done = false;
  static std::string data;
  
  if (done == false) {
      done = true;
      nfs = nfs_init_context();
      if (nfs == NULL) {
          throw std::runtime_error("Error initing nfs");
      }

      url = nfs_parse_url_dir(nfs,
              "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
      if (url == NULL) {
          throw std::runtime_error("Error parsing nfs url");
      }

      char *server = url->server;
      char* path = url->path;

      std::cout << "Mounting nfs" << std::endl;
      int ret = nfs_mount(nfs, server, path);
      if (ret != 0) {
          throw std::runtime_error("Error mounting nfs ret: " + std::to_string(ret));
      }

      data.reserve(10000);
      for (int i = 0; i < 10000; ++i)
          data += "a";
  }

  struct nfsfh* file_handle;
  int ret = nfs_open(nfs, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error open");
  }

  uint32_t write_size = data.size();
  std::cout << "Writing model with size: " << data.size()
    << " real write size: " << write_size << std::endl;

  ret = nfs_write_wrapper(
      nfs, file_handle, sizeof(uint32_t), data.size(), data.c_str());
  if (ret <= 0) {
    throw std::runtime_error("Error nfs_pwrite " + std::to_string(ret));
  }
  nfs_close(nfs, file_handle);
  std::cout << "Wrote and closed file" << std::endl;
    
  //nfs_destroy_context(nfs);
  //free(server); 
  //free(path); 
  //free(url); 
}

#if 0
void test_written_model(SparseLRModel& model) {
    NFSFile file("model0");
    char* model_data = new char[1024*1024*5];
    int ret = file.read(0, model_data, 3000000);

    SparseLRModel model2(0);
    SparseLRModel model3(0);

    FILE* fin = fopen("m0", "r");
    char* model_disk_data = new char[3*1024*1024];
    ret = fread(model_disk_data, 3*1024*1024, 1, fin);
    printf("read %d from disk\n", ret);
    fclose(fin);

    model3.loadSerialized(model_disk_data);

    model2.loadSerialized(model_data);

    std::cout << model3.checksum() << " " <<  model2.checksum() << " " << model.checksum() << std::endl;
    model.print();
    model2.print();
    model3.print();
    assert(model2 == model);
    assert(model3 == model);

    exit(0);
}
#endif

void PSSparseServerTaskEFS::erase_model(int model_version) {

  std::string filename = "/model" + std::to_string(model_version);

  std::cout << "Erasing file: " << filename << std::endl;

  int ret = nfs_unlink(nfs_, filename.c_str());
  if (ret != 0) {
    throw std::runtime_error("Error erasing model");
  }
}

void PSSparseServerTaskEFS::start() {
    nfs_ = nfs_init_context();
    if (nfs_ == NULL) {
        throw std::runtime_error("Error initing nfs");
    }

    url_ = nfs_parse_url_dir(nfs_,
            "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
    if (url_ == NULL) {
        throw std::runtime_error("Error parsing nfs url");
    }

    server_ = url_->server;
    path_ = url_->path;

    std::cout << "Mounting nfs" << std::endl;
    int ret = nfs_mount(nfs_, server_, path_);
    if (ret != 0) {
        throw std::runtime_error("Error mounting nfs ret: " + std::to_string(ret));
    }
}

void PSSparseServerTaskEFS::run(const Configuration& config) {

#if 0
  if (true) {
      int v = 0;
      while (1) {
          write_model3(v);
          v++;
      }
  }
  exit(0);
#endif

  start();

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
  //test_written_model(*lr_model);

  auto elapsed_time_us = get_time_us() - before;
  std::cout << "write_model bw MB/s: "
    << (model_size * 4 / 1024.0 / 1024 / elapsed_time_us * 1000 * 1000)
    << std::endl;

  while (1) {
    // we sit in a loop checking gradients and updating model

    auto time1 = get_time_us();
    auto grad = check_gradient();
    auto time2 = get_time_us();
    apply_to_model(grad);
    auto time3 = get_time_us();

    if (model_version >= 5) {
        erase_model(model_version - 5);
    }
    write_model(model_version++);
    auto time4 = get_time_us();
    std::cout
      << "Get gradient (us): " << time2 - time1
      << " Update model (us): " << time3-time2
      << " Write model (us): " << time4-time3
      << std::endl;
  }
}

} // namespace cirrus
