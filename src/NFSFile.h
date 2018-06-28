#ifndef _NFSFILE_H_
#define _NFSFILE_H_

#include <string>

#include <libnfs.h>

namespace cirrus {

class NFSFile {
  public:
    explicit NFSFile(const std::string& filename);
    NFSFile(struct nfs_context*, const std::string& filename);
    virtual ~NFSFile();

    int write(uint32_t offset, const char* data, uint32_t data_size);
    int read(uint32_t offset, char* data_output, uint32_t data_size);

  private:
    int write_(uint32_t offset, const char* data, uint32_t data_size);
    int read_(uint32_t offset, char* data_output, uint32_t data_size);

    std::string filename;
    struct nfs_context *nfs = nullptr;
    struct nfs_url *url = nullptr;
    struct nfsfh* file_handle = nullptr;
    
    struct {
      char *server = nullptr;
      char *path = nullptr;
      uint32_t mount_port;
      int is_finished;
    } client;

    bool created_nfs = false;
};

}  // namespace cirrus

#endif  // _NFSFILE_H_
