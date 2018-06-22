#include <NFSls.h>

#include <iostream>

namespace cirrus {

NFSls::NFSls(const std::string& path) :
  path(path) {

  std::cout << "Starting NFSls" << std::endl;

  nfs = nfs_init_context();
  if (nfs == NULL) {
    throw std::runtime_error("Error initing nfs");
  }
  
  url = NULL;
  url = nfs_parse_url_dir(nfs, "nfs://fs-ac79ac05.efs.us-west-2.amazonaws.com/?version=4&nfsport=2049");
  if (url == NULL) {
    throw std::runtime_error("Error parsing nfs url");
  }

  client.server = url->server;
  client.path = url->path;
  client.is_finished = 0;

  std::cout << "Mounting nfs" << std::endl;
  int ret = nfs_mount(nfs, client.server, client.path);
  if (ret != 0) {
    throw std::runtime_error("Error mounting nfs");
  }
}

std::vector<std::pair<std::string, uint64_t>> NFSls::do_ls() {
  std::cout << "Nfs open dir: " << path.c_str() << std::endl;
  int ret = nfs_opendir(nfs, path.c_str(), &nfsdir);
  if (ret != 0) {
    throw std::runtime_error("nfs open dir failed");
  }

  std::vector<std::pair<std::string, uint64_t>> result;
  struct nfsdirent *nfsdirent;
  std::cout << "Reading dir nfs" << std::endl;
  while((nfsdirent = nfs_readdir(nfs, nfsdir)) != NULL) {
    if (!strcmp(nfsdirent->name, ".") || !strcmp(nfsdirent->name, "..")) {
      continue;
    }

    struct nfs_stat_64 st;
    
    char dir_file_path[1024];
    sprintf(dir_file_path, "%s/%s", path.c_str(), nfsdirent->name);
    std::cout << "efs path: " << dir_file_path << " nfsdirent->name: " << nfsdirent->name << std::endl;
    ret = nfs_stat64(nfs, dir_file_path, &st);
    if (ret != 0) {
      fprintf(stderr, "Failed to stat(%s) %s\n", dir_file_path, nfs_get_error(nfs));
      continue;
    }
    std::cout << "path: " << dir_file_path << " size: " << st.nfs_size << std::endl;
    result.push_back(std::make_pair(std::string(nfsdirent->name), st.nfs_size));
    printf(" %s  result size: %d\n", nfsdirent->name, result.size());
  }
  nfs_closedir(nfs, nfsdir);

  return result;
}

NFSls::~NFSls() {
  free(client.server);
  free(client.path);
  if (nfs != NULL) {
    nfs_destroy_context(nfs);
  }
}

}  // namespace cirrus
