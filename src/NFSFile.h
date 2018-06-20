#ifndef _NFSFILE_H_
#define _NFSFILE_H_

#include <string>

class NFSFile {
  public:
    NFSFile(const std::string& filename);
    bool write(uint32_t offset, const char* data, uint32_t data_size);

  private:
    std::string filename;
    struct nfs_context *nfs = nullptr;
    struct nfs_url *url = nullptr;
    struct nfsfh* file_handle = nullptr;
    
    struct {
      char *server;
      char *path;
      uint32_t mount_port;
      int is_finished;
    } client;
};

#endif  // _NFSFILE_H_
