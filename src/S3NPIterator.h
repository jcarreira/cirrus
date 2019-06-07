#ifndef _S3_NPITERATOR_H_
#define _S3_NPITERATOR_H_

#include <CircularBuffer.h>
#include <Configuration.h>
#include <S3Client.h>
#include <S3Iterator.h>
#include <Serializers.h>
#include <SparseDataset.h>
#include <Synchronization.h>
#include <config.h>

#include <semaphore.h>
#include <list>
#include <mutex>
#include <queue>
#include <thread>

namespace cirrus {

class S3NPIterator : public S3Iterator {
 public:
  S3NPIterator(uint64_t left_id,
                   uint64_t right_id,
                   const Configuration& c,
                   uint64_t s3_rows,
                   uint64_t minibatch_rows,
                   bool use_label = true,
                   int worker_id = 0,
                   bool random_access = true,
                   bool has_labels = true);

  std::shared_ptr<SparseDataset> getNext() override;

 private:
  uint64_t getObjId(uint64_t left, uint64_t right);

  uint64_t left_id, right_id;

  std::shared_ptr<S3Client> s3_client;
  uint64_t s3_rows;
  uint64_t minibatch_rows;

  std::map<int, std::string> list_strings;  // strings from s3

  bool use_label;  // whether the dataset has labels or not
  int worker_id = 0;

  std::default_random_engine re;
  bool random_access = true;
  uint64_t current = 0;
};

}  // namespace cirrus

#endif  // _S3_NPITERATOR_H_
