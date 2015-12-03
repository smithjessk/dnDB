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

struct test {
  char *data;

  test () {
    data = (char *)malloc(512);
  }

  test (char *start) {
    mempcpy(this, start, 512);
  }

  zmq::message_t generate_message() {
    zmq::message_t msg(512);
    mempcpy((void *) msg.data(), this, 512);
    return msg;
  }
};

struct query {
  long id;
  query_type type;
  bool successful;
  long data_size = 0;
  std::string data;

  query (long id, query_type qt) {
    this->id = id;
    this->type = qt;
    this->successful = false;
    this->data_size = 0;
    this->data = "";
  }

  query (char *start) {
    id = *((long*) start);
    start += sizeof(long);

    std::cout << "id = " << id << std::endl;

    type = *((query_type*) start);
    start += sizeof(query_type);

    std::cout << "type = " << type << std::endl;

    successful = *((bool*) start);
    start += sizeof(bool);

    std::cout << "successful = " << successful << std::endl;

    data_size = *((long *) start);
    data_size = ntohl(data_size);
    start += sizeof(long);

    std::cout << "data_size = " << data_size << std::endl;

    data = std::string(start, data_size);
    std::cout << "data = " << data << std::endl;
  }

  long get_total_size() {
    return sizeof(long) + sizeof(query_type) + sizeof(bool) + data.size() + 1;
  }

  zmq::message_t generate_message() {
    zmq::message_t msg(get_total_size());
    memcpy((void *) msg.data(), this, get_total_size());
    char *begin = (char *) msg.data();
    char *loc_of_data = begin + 2 * sizeof(long) + sizeof(query_type) +
      sizeof(bool);
    memcpy((void *) loc_of_data, data.c_str(), data_size);
    bool *bool_begin = (bool *) (begin + sizeof(long) + sizeof(query_type));
    char *data_begin = ((char*) bool_begin) + sizeof(bool) + sizeof(long);
    std::string s(data_begin, data_size);
    std::cout << "s = " << s << std::endl;
    std::cout << "first char = " << (int) s[0] << std::endl;
    return msg;
  }
};

struct query_response {
  long id;
  bool successful;
  char *data;

  query_response(long id) {
    this->id = id;
    successful = false;
    data = NULL;
  }
};

#endif // QUERY_TYPES_H