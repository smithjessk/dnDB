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
  std::string data;

  query (long id, query_type qt) {
    this->id = id;
    this->type = qt;
    this->successful = false;
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

    data = *((std::string *) start);

    std::cout << "data = " << data << std::endl;
  }

  long get_total_size() {
    return sizeof(long) + sizeof(query_type) + sizeof(bool) + data.size() + 1;
  }

  zmq::message_t generate_message() {
    zmq::message_t msg(get_total_size());
    long size = get_total_size();
    std::printf("total size = %lu\n", size);
    mempcpy((void *) msg.data(), this, size);
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