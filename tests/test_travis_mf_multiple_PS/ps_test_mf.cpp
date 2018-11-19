// Code here starts a parameter server on localhost, port 1338

#include <Configuration.h>
#include <Tasks.h>

cirrus::Configuration config = cirrus::Configuration("configs/jester.cfg");
int main(int argc, char* argv[]) {
  std::vector<uint64_t> ports;
  ports.push_back(std::stoi(argv[1]));
  std::vector<std::string> ips{"127.0.0.1"};

  cirrus::PSSparseServerTask st(
      (1 << config.get_model_bits()) + 1, config.get_minibatch_size(),
      config.get_minibatch_size(), config.get_num_features(), 2, 1, config, ips,
      ports);
  st.run(config);
}
