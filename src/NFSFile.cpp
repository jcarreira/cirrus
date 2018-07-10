#include <NFSFile.h>

#include <libnfs.h>

#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

namespace cirrus {

NFSFile::NFSFile(struct nfs_context * nfs,  const std::string& filename) :
  nfs(nfs),
  filename(filename),
  url(url) {
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

  url = nfs_parse_url_dir(nfs,
      "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
  if (url == NULL) {
    throw std::runtime_error("Error parsing nfs url");
  }

  client.server = url->server;
  client.path = url->path;

  int ret = nfs_mount(nfs, client.server, client.path);
  if (ret != 0) {
    throw std::runtime_error("Error mounting nfs");
  }

  ret = nfs_open(nfs, filename.c_str(), O_CREAT | O_RDWR, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error opening nfs file");
  }
}

/**
  * We have to write in chunks of ~1M bytes at a time otherwise nfs closes
  * the connection
  */
int NFSFile::write(uint32_t offset, const char* data, uint32_t data_size) {
  uint32_t chunk_size = 1000000;
  
  int written = 0;

  while (written < data_size) {
    uint32_t to_write = std::min(chunk_size, data_size - written);
    int ret = write_(offset + written, data + written, to_write);
    written += ret;
  }

  return written;
}

int NFSFile::write_(uint32_t offset, const char* data, uint32_t data_size) {
  int ret = nfs_pwrite(nfs, file_handle, offset, data_size, data);
  std::cout << "NFSFile::write(off:"<<offset<<", data..., data_size:"<<data_size<<") = " << ret << std::endl;
  if (ret <= 0) {
    throw std::runtime_error("Error writing nfs file " + std::to_string(ret));
  }
  return ret;
}

/**
  * We have to read in chunks of ~1M bytes at a time otherwise
  * we get garbage from NFS
  */
int NFSFile::read(uint32_t offset, char* data, uint32_t data_size) {
  uint32_t chunk_size = 1000000;
  
  uint32_t bytes_read = 0;

  while (bytes_read < data_size) {
    uint32_t to_read = std::min(chunk_size, data_size - bytes_read);
    std::cout << "bytes_read: " << bytes_read << " data_size: " << data_size << " to_read: " << to_read << std::endl;
    int ret = read_(offset + bytes_read, data + bytes_read, to_read);
    if (ret == 0) { // does this indicate EOF?
      break;
    }
    bytes_read += ret;
  }
  return bytes_read;
}

int NFSFile::read_(uint32_t offset, char* data_out, uint32_t data_size) {
  int total_read = 0;

try_again:
  int ret = nfs_pread(nfs, file_handle, offset, data_size, data_out);
  std::cout << "NFSFile::read(off:"<<offset<<", data..., data_size:"<<data_size<<") = " << ret << std::endl;
  if (ret < 0) {
    throw std::runtime_error("Error reading nfs file ret: " + std::to_string(ret));
  } else if (ret == -11) {
    goto try_again;
  }
  return ret;
}

NFSFile::~NFSFile() {
  if (client.server)
    free(client.server);
  if (client.path)
    free(client.path);

  int ret = nfs_close(nfs, file_handle);
  if (ret != 0) {
      std::cout << "Error closing nfs file ret: " << std::endl;
      exit(-1);
  }

  if (created_nfs && nfs != NULL) {
    nfs_destroy_context(nfs);
  }
}

}  // namespace cirrus

