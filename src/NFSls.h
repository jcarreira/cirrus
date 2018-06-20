#ifndef _NFSLS_H_
#define _NFSLS_H_

#include <string>
#include <vector>
#include <stdexcept>
#include <libnfs.h>
#include <cstring>

class NFSls {
 public:
    NFSls(const std::string& path);

    std::vector<std::string> do_ls();
 private:
    struct nfs_context *nfs = nullptr;
    struct nfs_url *url = nullptr;
    struct nfsdir *nfsdir = nullptr;
    
    struct {
      char *server;
      char *path;
      uint32_t mount_port;
      int is_finished;
    } client;

    std::string path;
};

NFSls::NFSls(const std::string& path) :
  path(path) {
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

  int ret = nfs_mount(nfs, client.server, client.path);
  if (ret != 0) {
    throw std::runtime_error("Error mounting nfs");
  }

}

std::vector<std::string> NFSls::do_ls() {
  int ret = nfs_opendir(nfs, path.c_str(), &nfsdir);
  if (ret != 0) {
    throw std::runtime_error("nfs open dir failed");
  }

  std::vector<std::string> result;
  struct nfsdirent *nfsdirent;
  while((nfsdirent = nfs_readdir(nfs, nfsdir)) != NULL) {
    char path[1024];

    if (!strcmp(nfsdirent->name, ".") || !strcmp(nfsdirent->name, "..")) {
      continue;
    }

    sprintf(path, "%s/%s", "/", nfsdirent->name);
    result.push_back(nfsdirent->name);
    struct nfs_stat_64 st;
    ret = nfs_stat64(nfs, path, &st);
    if (ret != 0) {
      fprintf(stderr, "Failed to stat(%s) %s\n", path, nfs_get_error(nfs));
      continue;
    }
    printf(" %s\n", nfsdirent->name);
  }
  nfs_closedir(nfs, nfsdir);
}

#endif  // _NFSLS_H_
