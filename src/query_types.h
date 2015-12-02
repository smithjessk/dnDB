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
  long id;
  query_type type;
  bool successful;
  long data_size;
  void *data;

  query (long id, query_type qt) {
    this->id = id;
    this->type = qt;
    this->successful = false;
    this->data_size = 0;
    this->data = NULL;
  }

  query (char *start) {
    id = *((long*) start);
    start += sizeof(long);

    type = *((query_type*) start);
    start += sizeof(query_type);

    successful = *((bool*) start);
    start += sizeof(bool);

    data_size = *((long*) start);
    data_size = ntohl(data_size);
    start += sizeof(long);

    data = (void*) start;
  }

  long get_total_size() {
    return 2 * sizeof(long) + sizeof(query_type) + sizeof(bool) + data_size + 
      sizeof(void *);
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