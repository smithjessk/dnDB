#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

#include <sstream>
#include <mutex>
#include <condition_variable>
#include <arpa/inet.h>

#include "../deps/zmq.hpp"

struct table_connection {
  std::string address;
  bool waiting_for_reply;
  std::mutex mutex;
  std::condition_variable cv;
  zmq::context_t context;
  zmq::socket_t socket;

  table_connection(int port)
    : context(1),
    socket(context, ZMQ_REQ) {
      std::stringstream ss;
      ss << "tcp://localhost:";
      ss << port;
      ss >> address;
      socket.connect(address);
      waiting_for_reply = false;
  }
};

enum query_type {CREATE, READ, UPDATE, DELETE};

struct query {
  uint32_t id;
  query_type type;
  bool successful;
  uint32_t data_size = 0;
  std::string data;

  query (uint32_t id, query_type qt) {
    this->id = id;
    this->type = qt;
    this->successful = false;
    this->data_size = 0;
    this->data = "";
  }

  query (char *start) {
    id = *((uint32_t*) start);
    start += sizeof(uint32_t);

    type = *((query_type*) start);
    start += sizeof(query_type);

    successful = *((bool*) start);
    start += sizeof(bool);

    uint8_t *temp_ptr = (uint8_t *) start;
    data_size = (temp_ptr[0]<<0) | (temp_ptr[1]<<8) | (temp_ptr[2]<<16) | 
      (temp_ptr[3]<<24);
    start += sizeof(uint32_t);

    data = std::string(start, data_size);
  }

  void set_data(std::string s) {
    this->data_size = s.size();
    this->data = s;
  }

  uint32_t get_total_size() {
    return sizeof(uint32_t) + sizeof(query_type) + sizeof(bool) + sizeof(uint32_t) + data.size() + 1;
  }

  zmq::message_t generate_message() {
    zmq::message_t msg(get_total_size());
    char *ptr = (char *) msg.data();

    memcpy((void *) ptr, &(this->id), sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy((void *) ptr, &(this->type), sizeof(query_type));
    ptr += sizeof(query_type);

    memcpy((void *) ptr, &(this->successful), sizeof(bool));
    ptr += sizeof(bool);

    memcpy((void *) ptr, &(this->data_size), sizeof(uint32_t));
    ptr += sizeof(uint32_t);

    memcpy((void *) ptr, data.c_str(), data_size);
    return msg;
  }
};

struct query_response {
  uint32_t id;
  bool successful;
  char *data;

  query_response(uint32_t id) {
    this->id = id;
    successful = false;
    data = NULL;
  }
};

#endif // QUERY_TYPES_H