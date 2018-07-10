#ifndef _NFSLS_H_
#define _NFSLS_H_

#include <string>
#include <vector>
#include <stdexcept>
#include <libnfs.h>
#include <cstring>

namespace cirrus {

class NFSls {
 public:
    NFSls(struct nfs_context* nfs_, const std::string& path);
    NFSls(const std::string& path);
    virtual ~NFSls();

    std::vector<std::pair<std::string, uint64_t>> do_ls();
 public:
    struct nfs_context* nfs = nullptr;
    struct nfs_url* url = nullptr;
    struct nfsdir* nfsdir = nullptr;

    std::string path;

    bool from_scratch = false;
};

}  // namespace cirrus

#endif  // _NFSLS_H_
