#include <unistd.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <Configuration.h>
#include <InputReader.h>
#include <PSSparseServerInterface.h>
#include <SparseLRModel.h>
#include <Tasks.h>

#include "S3.h"
#include "SGD.h"
#include "Serializers.h"
#include "Utils.h"

using namespace cirrus;

void load_serialized_indices(char* mem_begin,
                             std::vector<std::vector<int>>& slice_indices) {
  const char* mem = mem_begin;

  int num_slices = load_value<int32_t>(mem);
  slice_indices.clear();
  slice_indices.reserve(num_slices);
  for (int i = 0; i < num_slices; ++i) {
    int len = load_value<int32_t>(mem);
    std::vector<int> slice_i;
    slice_i.reserve(len);
    for (int j = 0; j < len; ++j) {
      int temp = load_value<int32_t>(mem);
      slice_i.push_back(temp);
    }
    slice_indices.push_back(slice_i);
  }
}

int main_worker(int worker_id) {
  cirrus::Configuration config = cirrus::Configuration("configs/lda_test.cfg");

  cirrus::s3_initialize_aws();

  // Download the LDAStatistics from S3
  std::shared_ptr<cirrus::S3Client> s3_client =
      std::make_shared<cirrus::S3Client>();
  std::string obj_id_str =
      std::to_string(hash_f(std::to_string(worker_id).c_str())) + "-LDA";
  std::ostringstream* s3_obj = s3_client->S3Client::s3_get_object_ptr(
      obj_id_str, config.get_s3_bucket());

  // initialize the model with LDAStatistics
  std::unique_ptr<LDAModel> model;
  model.reset(new LDAModel(s3_obj->str().c_str()));
  delete s3_obj;

  // Connect to server
  std::unique_ptr<PSSparseServerInterface> psi =
      std::make_unique<PSSparseServerInterface>("127.0.0.1", 1337);
  psi->connect();

  // load the pre-cached indices from server
  std::vector<std::vector<int>> slice_indices;
  char* slice_indices_mem = psi->get_slices_indices(worker_id);
  load_serialized_indices(slice_indices_mem, slice_indices);
  delete slice_indices_mem;

  uint32_t to_receive_size, uncompressed_size, gradient_size;
  int total_sampled_tokens = 0, cur = 0;
  int num_runs = 100000 / config.get_slice_size() + 1;

  while (1) {
    if (cur >= num_runs) {
      // send ll to server
      double ll_to_send = model->compute_ll_ndt();
      psi->update_ll_ndt(worker_id, ll_to_send);
      cur = 0;
    }

    cur += 1;

    // load one word slice from server
    char* partial_model =
        psi->get_lda_model(worker_id, to_receive_size, uncompressed_size);

    // update current model with vocab slice
    model->update_model(partial_model, to_receive_size, uncompressed_size,
                        psi->slice_id);

    // sampling
    char* gradient_mem;
    gradient_mem = model->sample_model(
        total_sampled_tokens, slice_indices[psi->slice_id], gradient_size);
    psi->send_lda_update(gradient_mem, total_sampled_tokens, 0, gradient_size);
  }

  cirrus::s3_shutdown_aws();
  return 0;
}
