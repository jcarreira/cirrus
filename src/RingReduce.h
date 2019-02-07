#include <vector>

namespace cirrus {

class RingReduce {
  public:
    RingReduce(const Configuration& config);

    /**
      * Given a model it reduces it across all workers
      * and returns the new reduced model
      */
    std::vector<FEATURE_TYPE> reduce(const std::vector<FEATURE_TYPE>& params);

  private:
    Configuration config;
};

};  // namespace cirrus
