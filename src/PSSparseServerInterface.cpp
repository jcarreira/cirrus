#include <cassert>
#include <stdexcept>

#include <PSSparseServerInterface.h>
#include <Constants.h>
#include <MFModel.h>
#include <Checksum.h>
#include <Constants.h>
#include <CreateTensorMessage.h>
#include <CreateTensorMessageReply.h>
#include <AddTensorMessage.h>
#include <AddTensorMessageReply.h>
#include <GetTensorMessage.h>
#include <GetTensorMessageReply.h>
#include <GetSparseTensorMessage.h>
#include <GetSparseTensorMessageReply.h>

uint32_t GET_TENSOR_MSG_VAR = GET_TENSOR_MSG;
uint32_t GET_SPARSE_TENSOR_MSG_VAR = GET_SPARSE_TENSOR_MSG;
uint32_t CREATE_TENSOR_MSG_VAR = CREATE_TENSOR_MSG;
uint32_t ADD_TENSOR_MSG_VAR = ADD_TENSOR_MSG;
//#define DEBUG

#define MAX_MSG_SIZE (1024*1024)

namespace cirrus {

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

void PSSparseServerInterface::setStatus(uint32_t id, uint32_t status) {
  std::cout << "Setting status id: " << id << " status: " << status << std::endl;
  uint32_t data[3] = {SET_TASK_STATUS, id, status};
  if (send_all(sock, data, sizeof(uint32_t) * 3) == -1) {
    throw std::runtime_error("Error setting task status");
  }
}

uint32_t PSSparseServerInterface::getStatus(uint32_t id) {
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

bool PSSparseServerInterface::createTensor2D(const std::string& tensor_name,
    std::pair<uint32_t, uint32_t> tensor_dims) {
  CreateTensorMessage create_msg(tensor_name,
                                 std::vector<uint32_t>{tensor_dims.first,
                                                       tensor_dims.second});
  throw "fix";
  false;
}

bool PSSparseServerInterface::createTensor1D(const std::string& tensor_name,
    uint32_t tensor_dim) {
  CreateTensorMessage create_msg(tensor_name, tensor_dim);
  
  if (send_all(sock, const_cast<char*>(create_msg.getData()),
        create_msg.getTotalDataSize()) == -1) {
    throw std::runtime_error("Error sending create tensor msg");
  }

  CreateTensorMessageReply create_msg_reply;
  // receive boolean reply
  if (read_all(sock,
        const_cast<char*>(create_msg_reply.getReturnData()),
        create_msg_reply.getReturnSize()) == 0) {
    throw std::runtime_error("Error getting create_msg_reply");
  }

  return create_msg_reply.getBool();
}

bool PSSparseServerInterface::addTensor1D(const std::string& tensor_name,
        const SparseTensor1D& sparse_tensor) {
  AddTensorMessage add_tensor_msg(tensor_name, sparse_tensor);
 
//  // 1. send tensor size 
//  if (send_all(sock, add_tensor_msg.getTensorSizeData(),
//        add_tensor_msg.getTensorSizeSize()) == -1) {
//      throw std::runtime_error("Error sending update tensor msg");
//  }
//
//  // 2. send tensor
//  if (send_all(sock, const_cast<char*>(add_tensor_msg.getTensorData()),
//        add_tensor_msg.getTensorSize()) == -1) {
//      throw std::runtime_error("Error sending update tensor msg");
//  }

  AddTensorMessageReply add_tensor_msg_reply;
  if (read_all(sock, const_cast<char*>(add_tensor_msg_reply.getReturnData()),
              add_tensor_msg_reply.getReturnSize()) == 0) {
      throw std::runtime_error("Error getting add_tensor_msg_reply");
  }

  return add_tensor_msg_reply.getBool();
}

Tensor1D PSSparseServerInterface::getTensor1D(const std::string& tensor_name) {
  GetTensorMessage get_tensor_msg(tensor_name);

  // 1. send operation id
  if (send_all(sock, &GET_TENSOR_MSG_VAR, sizeof(GET_TENSOR_MSG_VAR)) == -1) {
      throw std::runtime_error("Error send GET_TENSOR_MSG");
  }

  // 2. send tensor name
  if (send_all(sock, const_cast<char*>(get_tensor_msg.getNameData()),
        get_tensor_msg.getNameSize() + 1) == -1) {
      throw std::runtime_error("Error sending tensor name");
  }

  GetTensorMessageReply get_tensor_msg_reply;
  if (read_all(sock, get_tensor_msg_reply.getTensorSizeData(),
              sizeof(uint32_t)) == 0) {
      throw std::runtime_error("Error getting get_tensor_msg_reply");
  }

  get_tensor_msg_reply.reserveTensorData();
  if (read_all(sock, get_tensor_msg_reply.getTensorData(),
              get_tensor_msg_reply.getTensorSize()) == 0) {
      throw std::runtime_error("Error getting get_tensor_msg_reply");
  }

  return Tensor1D(get_tensor_msg_reply.getTensorData());
}

SparseTensor1D PSSparseServerInterface::getSparseTensor1D(const std::string& tensor_name,
        const std::vector<uint32_t>& indexes) {
  // 1. send operation id
  if (send_all(sock, &GET_SPARSE_TENSOR_MSG_VAR, sizeof(GET_SPARSE_TENSOR_MSG_VAR)) == -1) {
      throw std::runtime_error("Error send GET_TENSOR_MSG");
  }

  GetSparseTensorMessage msg(tensor_name, indexes);
  // 2. send tensor name
  if (send_all(sock, const_cast<char*>(msg.getData()),
        msg.getDataSize()) == -1) {
      throw std::runtime_error("Error sending tensor name");
  }

  GetSparseTensorMessageReply msg_reply(indexes);
  if (read_all(sock, msg_reply.getData(),
              msg_reply.getSize()) == 0) {
      throw std::runtime_error("Error getting msg_reply");
  }

  return SparseTensor1D(msg_reply.getData());
}

SparseTensor2D PSSparseServerInterface::getSparseTensor2D(const std::string& tensor_name,
        const std::vector<std::pair<uint32_t, uint32_t>>& indexes) {
  throw "fix";
  return SparseTensor2D(nullptr);
}

} // namespace cirrus

