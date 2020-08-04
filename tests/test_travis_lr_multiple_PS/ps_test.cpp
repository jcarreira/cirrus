#include <Configuration.h>
#include <Tasks.h>

cirrus::Configuration config = cirrus::Configuration("configs/test_config.cfg");
int main(int argc, char* argv[]) {
  std::vector<std::string> ips{"127.0.0.1"};
  std::vector<uint64_t> ports{std::stoi(argv[1])};

  cirrus::PSSparseServerTask st(
      (1 << config.get_model_bits()) + 1, config.get_minibatch_size(),
      config.get_minibatch_size(), config.get_num_features(), 2, 1, config, ips,
      ports);
  st.run(config);
}
