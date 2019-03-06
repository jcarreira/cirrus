#include <Configuration.h>
#include <Tasks.h>
#include "S3.h"

int main() {
  cirrus::Configuration config = cirrus::Configuration("configs/lda_test.cfg");

  cirrus::s3_initialize_aws();
  cirrus::PSSparseServerTask st(
      (1 << config.get_model_bits()) + 1, config.get_minibatch_size(),
      config.get_minibatch_size(), config.get_num_features(), 2, 1, "127.0.0.1",
      1337);
  st.run(config);
  cirrus::s3_shutdown_aws();

  return 0;
}
