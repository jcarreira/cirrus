#include <NFSFile.h>

#include <libnfs.h>

#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

namespace cirrus {

NFSFile::NFSFile(struct nfs_context * nfs, const std::string& filename) :
  filename(filename) {
  throw "not supported";
  int ret = nfs_open(nfs, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error opening nfs file");
  }
}

NFSFile::NFSFile(const std::string& filename) :
  filename(filename) {

  std::cout << "NFSFile filename: " << filename << std::endl;

  created_nfs = true;
  nfs = nfs_init_context();
  if (nfs == NULL) {
    throw std::runtime_error("Error initing nfs");
  }
  url = NULL;
  url = nfs_parse_url_dir(nfs,
      "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
  if (url == NULL) {
    throw std::runtime_error("Error parsing nfs url");
  }

  client.server = url->server;
  client.path = url->path;
  client.is_finished = 0;

  int ret = nfs_mount(nfs, client.server, client.path);
  if (ret != 0) {
    throw std::runtime_error("Error mounting nfs");
  }

  ret = nfs_open(nfs, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error opening nfs file");
  }
}

bool NFSFile::write(uint32_t offset, const char* data, uint32_t data_size) {
  std::cout << "NFSFile::write(off:"<<offset<<", data..., data_size:"<<data_size<<")"<<std::endl;
  int ret = nfs_pwrite(nfs, file_handle, offset, data_size, data);
  if (ret != 0) {
    throw std::runtime_error("Error writing nfs file " + std::to_string(ret));
  }
  return true;
}

bool NFSFile::read(uint32_t offset, char* data_out, uint32_t data_size) {
  std::cout << "NFSFile::read(off:"<<offset<<", data..., data_size:"<<data_size<<")"<<std::endl;
  int ret = nfs_pread(nfs, file_handle, offset, data_size, data_out);
  if (ret != 0) {
    throw std::runtime_error("Error reading nfs file ret: " + std::to_string(ret));
  }
  return true;
}

NFSFile::~NFSFile() {
  if (client.server)
    free(client.server);
  if (client.path)
    free(client.path);

  nfs_close(nfs, file_handle);
  if (created_nfs && nfs != NULL) {
    nfs_destroy_context(nfs);
  }

}

}  // namespace cirrus

