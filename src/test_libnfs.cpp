#include <iostream>

#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <libnfs.h>
#include <cstring>

int main() {
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
    throw std::runtime_error("Error mounting nfs");
  }

  struct nfsfh* file_handle;
  ret = nfs_open(nfs, "/model0", O_CREAT | O_RDWR | O_DIRECT, &file_handle);
  if (ret != 0) {
    throw std::runtime_error("Error open");
  }

  char* dummy_data = new char[10 * 1024 * 1024];
  memset(dummy_data, 0, 10 * 1024 * 1024);

  for (int i = 100; i < 10 * 1024 * 1024; i += 100) {


    for (int j = i - 100; j < i; ++j) {
      dummy_data[j] = 'A';
    }

    std::cout << "writing size: " << i << std::endl;
    ret = nfs_pwrite(nfs, file_handle, 0, i, dummy_data);
    if (ret <= 0) {
      continue;
      throw std::runtime_error("Error nfs_write " + std::to_string(ret));
    } else {
      printf("size: %d worked\n", i);
    }

    nfs_fsync(nfs, file_handle);
  }
  nfs_close(nfs, file_handle);


    
  nfs_destroy_context(nfs);
  free(server); 
  free(path); 
  free(url); 
}
