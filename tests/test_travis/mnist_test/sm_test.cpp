#include <Configuration.h>
#include <Tasks.h>

#define NUM_WORKERS (2) // number of workers
#define CONFIG_PATH ("configs/softmax_config.cfg") // path to config file

cirrus::Configuration config =
    cirrus::Configuration(CONFIG_PATH);
int main() {
  cirrus::PSSparseServerTask st(
      (1 << config.get_model_bits()) + 1, config.get_minibatch_size(),
      config.get_minibatch_size(), config.get_num_features(), NUM_WORKERS, PS_SPARSE_SERVER_TASK_RANK, PS_IP,
      PS_PORT);
  st.run(config);
}
