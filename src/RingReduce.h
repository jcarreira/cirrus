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
    void start_server(unsigned long int nworkers, int worker_id);
    void send_to_neighbor(int neighbor_id, const std::vector<float>&);
    void connect_to_neighbor(std::string ip, int port);
    void receive_from_neighbor();

    std::vector<std::pair<std::string,int>> workers;
    int worker_id;

    int neighbor_receive;
    int neighbor_send;

    std::mutex receive_mutex;

    //const std::vector<float>& params;
};

};  // namespace cirrus
