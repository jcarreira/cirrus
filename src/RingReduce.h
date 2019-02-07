#include <vector>
#include <string>
#include <utility>
#include <mutex>

namespace cirrus {

class RingReduce {
  public:
    RingReduce(const std::vector<std::pair<std::string, int>>& workers,
               int worker_id);

    /**
      * Given a model it reduces it across all workers
      * and returns the new reduced model
      */
    std::vector<float> reduce(
        const std::vector<float>& params);

  private:
    std::pair<int, int> calculate_receive_data(int, const std::vector<float>& params, int);
    //std::pair<int, int> calculate_data(int, const std::vector<float>& params);
    void start_server(unsigned long int nworkers, int worker_id);
    void send_to_neighbor(const std::vector<float>&, int iter);
    void connect_to_neighbor();
    void receive_from_neighbor(const std::vector<float>& params, int iter);

    void test();

    std::vector<std::pair<std::string,int>> workers;
    int worker_id;

    int neighbor_receive = 0;
    int neighbor_send = 0;
    int neighbor_send_id = 0;
    int neighbor_receive_id = 0;

    std::mutex receive_mutex;

    std::vector<float> result;
};

};  // namespace cirrus
