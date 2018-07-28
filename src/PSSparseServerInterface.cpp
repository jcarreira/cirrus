#include <cassert>
#include <stdexcept>
#include "PSSparseServerInterface.h"
#include "Constants.h"
#include "MFModel.h"
#include "Checksum.h"
#include "Constants.h"
#include "common/schemas/PSMessage_generated.h"
#include "common/schemas/WorkerMessage_generated.h"

//#define DEBUG

#define MAX_MSG_SIZE (1024*1024)

namespace cirrus {

static const int initial_buffer_size = 50;

PSSparseServerInterface::PSSparseServerInterface(const std::string& ip, int port) :
  ip(ip), port(port) {

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw std::runtime_error("Error when creating socket.");
  }
  int opt = 1;
  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt))) {
    throw std::runtime_error("Error setting socket options.");
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) != 1) {
    throw std::runtime_error("Address family invalid or invalid "
        "IP address passed in");
  }
  // Save the port in the info
  serv_addr.sin_port = htons(port);
  std::memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

  // Connect to the server
  if (::connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    throw std::runtime_error(
        "Client could not connect to server."
        " Address: " + ip + " port: " + std::to_string(port));
  }
}

PSSparseServerInterface::~PSSparseServerInterface() {
  if (sock != -1) {
    close(sock);
  }
}

void PSSparseServerInterface::send_lr_gradient(const LRSparseGradient& gradient) {
  PSSparseServerInterface::send_gradient(gradient, 
    message::WorkerMessage::ModelType_LOGISTIC_REGRESSION
    )
}

void PSSparseServerInterface::get_lr_sparse_model_inplace(const SparseDataset& ds, SparseLRModel& lr_model,
    const Configuration& config) {
#ifdef DEBUG
  std::cout << "Getting LR sparse model inplace" << std::endl;
#endif
  flatbuffers::FlatBufferBuilder builder(initial_buffer_size);

  // Make the index vector
  // We don't know the number of weights to start with
  unsigned char* msg = new unsigned char[MAX_MSG_SIZE];
  unsigned char* msg_start = msg;
  uint32_t num_bytes = 0;

  for (const auto& sample : ds.data_) {
    for (const auto& w : sample) {
      num_bytes += sizeof(w.first)
      store_value<uint32_t>(msg, w.first); // encode the index
    }
  }
  auto index_vec = builder.CreateVector(num_bytes, static_cast<unsigned char **> (&msg_start));
  
  auto sparse_msg = message::WorkerMessage::CreateSparseModelRequest(builder, 
    index_vec,
    message::WorkerMessage::ModelType_LOGISTIC_REGRESSION);

  builder.Finish(sparse_msg);

#ifdef DEBUG
  std::cout << "Sending sparse model request" << std::endl;
#endif
  send_flatbuffer(sock, &builder);
  
  // Get the message size and FlatBuffer message
  int msg_size;
  if (read_all(sock, &msg_size, sizeof(int)) == 0) {
    handle_failed_read(&req.poll_fd);
    continue;
  }
  char buf[msg_size];
  try {
    if (read_all(sock, &buf, msg_size) == 0) {
      throw std::runtime_error("Error reading message");
    }
  } catch (...) {
    throw std::runtime_error("Unhandled error");
  }

  auto msg = message::PSMessage::GetPSMessage(&buf)->payload_as_SparseModelResponse();

#ifdef DEBUG
  std::cout << "Loading model from memory" << std::endl;
#endif
  // build a truly sparse model and return
  // TODO: Can this copy be avoided?
  lr_model.loadSerializedSparse((FEATURE_TYPE*)buffer, 
    (uint32_t*)msg->model()->data(), msg->model()->size(), config);
}

SparseLRModel PSSparseServerInterface::get_lr_sparse_model(const SparseDataset& ds, const Configuration& config) {
  SparseLRModel model(0);
  get_lr_sparse_model_inplace(ds, model, config);
  return std::move(model);
}

std::unique_ptr<CirrusModel> PSSparseServerInterface::get_full_model(
    bool isCollaborative //XXX use a better argument here
    ) {
#ifdef DEBUG
  std::cout << "Getting full model isCollaborative: " << isCollaborative << std::endl;
#endif
  if (isCollaborative) {
    // 1. Send operation
    uint32_t operation = GET_MF_FULL_MODEL;
    send_all(sock, &operation, sizeof(uint32_t));

    uint32_t to_receive_size;
    read_all(sock, &to_receive_size, sizeof(uint32_t));
    std::cout << "Request sent. Receiving: " << to_receive_size << " bytes" << std::endl;

    char* buffer = new char[to_receive_size];
    read_all(sock, buffer, to_receive_size);
    
    std::cout
      << " buffer checksum: " << crc32(buffer, to_receive_size)
      << std::endl;

    // build a sparse model and return
    std::unique_ptr<CirrusModel> model = std::make_unique<MFModel>(
        (FEATURE_TYPE*)buffer, 0, 0, 0); //XXX fix this
    delete[] buffer;
    return model;
  } else {
    // 1. Send operation
    uint32_t operation = GET_LR_FULL_MODEL;
    send_all(sock, &operation, sizeof(uint32_t));
    //2. receive size from PS
    int model_size;
    if (read_all(sock, &model_size, sizeof(int)) == 0) {
      throw std::runtime_error("Error talking to PS");
    }
    char* model_data = new char[sizeof(int) + model_size * sizeof(FEATURE_TYPE)];
    char*model_data_ptr = model_data;
    store_value<int>(model_data_ptr, model_size);

    if (read_all(sock, model_data_ptr, model_size * sizeof(FEATURE_TYPE)) == 0) {
      throw std::runtime_error("Error talking to PS");
    }
    std::unique_ptr<CirrusModel> model = std::make_unique<SparseLRModel>(0);
    model->loadSerialized(model_data);

    delete[] model_data;
    return model;
  }
}

// Collaborative filtering

/**
  * This function needs to send to the PS a list of users and items
  * FORMAT of message to send is:
  * K item ids to send (uint32_t)
  * base user id (uint32_t)
  * minibatch size (uint32_t)
  * magic number (MAGIC_NUMBER) (uint32_t)
  * list of K item ids (K * uint32_t)
  */
SparseMFModel PSSparseServerInterface::get_sparse_mf_model(
    const SparseDataset& ds, uint32_t user_base, uint32_t minibatch_size) {
  flatbuffers::FlatBufferBuilder builder(initial_buffer_size);

  char* msg = new char[MAX_MSG_SIZE];
  char* msg_begin = msg; // need to keep this pointer to delete later
 
  store_value<uint32_t>(msg, user_base);
  store_value<uint32_t>(msg, minibatch_size);
  bool seen[17770] = {false};
  int num_bytes = sizeof(uint32_t) * 2;

  for (const auto& sample : ds.data_) {
    for (const auto& w : sample) {
      uint32_t movieId = w.first;
      //std::cout << "movieId: " << movieId << "\n";

      if (seen[movieId])
          continue;
      store_value<uint32_t>(msg, movieId);
      seen[movieId] = true;
      //store_value<uint32_t>(msg, movieId); // encode the index
      num_bytes += sizeof(uint32_t);
    }
  }

  auto id_vec = builder.CreateVector(num_bytes, static_cast<unsigned char **> (&msg_begin));
  
  auto sparse_msg = message::WorkerMessage::CreateSparseModelRequest(builder, 
    id_vec,
    message::WorkerMessage::ModelType_MATRIX_FACTORIZATION);

  builder.Finish(sparse_msg);

  send_flatbuffer(sock, &builder);
  
  // receive user vectors and item vectors
  // FORMAT here is
  // minibatch_size * user vectors. Each vector is user_id + user_bias + NUM_FACTORS * FEATURE_TYPE
  // num_item_ids * item vectors. Each vector is item_id + item_bias + NUM_FACTORS * FEATURE_TYPE

  // Get the message size and FlatBuffer message
  int msg_size;
  if (read_all(sock, &msg_size, sizeof(int)) == 0) {
    handle_failed_read(&req.poll_fd);
    continue;
  }
  char buf[msg_size];
  try {
    if (read_all(sock, &buf, msg_size) == 0) {
      throw std::runtime_error("Error reading message");
    }
  } catch (...) {
    throw std::runtime_error("Unhandled error");
  }

  auto msg = message::PSMessage::GetPSMessage(&buf)->payload_as_SparseModelResponse();

  // build a sparse model and return
  SparseMFModel model((FEATURE_TYPE*) msg->model->data, minibatch_size, item_ids_count);

  return std::move(model);
}

void PSSparseServerInterface::send_gradient(
    const ModelGradient& gradient,
    message::WorkerMessage::ModelType mt) {
  flatbuffers::FlatBufferBuilder builder(initial_buffer_size);
  int grad_size = gradient.getSerializedSize();
  unsigned char buf[grad_size];
  auto grad_vec = builder.CreateUninitializedVector(grad_size, static_cast<unsigned char **> (&buf));
  gradient.serialize(buf);
  auto grad_msg = message::WorkerMessage::CreateGradientMessage(builder, 
    grad_vec, 
    mt);
  builder.Finish(grad_msg);
#ifdef DEBUG
  std::cout << "Sending gradient" << std::endl;
#endif
  send_flatbuffer(sock, &builder);
}

void PSSparseServerInterface::send_mf_gradient(const MFSparseGradient& gradient) {
  PSSparseServerInterface::send_gradient(
    gradient,
    message::WorkerMessage::ModelType_MATRIX_FACTORIZATION
    )
}
  
void PSSparseServerInterface::set_status(uint32_t id, uint32_t status) {
  std::cout << "Setting status id: " << id << " status: " << status << std::endl;
  uint32_t data[3] = {SET_TASK_STATUS, id, status};
  if (send_all(sock, data, sizeof(uint32_t) * 3) == -1) {
    throw std::runtime_error("Error setting task status");
  }
}

uint32_t PSSparseServerInterface::get_status(uint32_t id) {
  uint32_t data[2] = {GET_TASK_STATUS, id};
  if (send_all(sock, data, sizeof(uint32_t) * 2) == -1) {
    throw std::runtime_error("Error getting task status");
  }
  uint32_t status;
  if (read_all(sock, &status, sizeof(uint32_t)) == 0) {
    throw std::runtime_error("Error getting task status");
  }
  return status;
}

} // namespace cirrus

