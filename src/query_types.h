#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

#include <sstream>
#include <mutex>
#include <condition_variable>
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
    std::printf("%p\n", start);
    id = *((long*) start);
    start += sizeof(long);

    std::printf("%p\n", start);
    type = *((query_type*) start);
    start += sizeof(query_type);

    std::printf("%p\n", start);
    successful = *((bool*) start);
    start += sizeof(bool);

    std::printf("%p\n", start);
    data_size = *((long*) start);
    start += sizeof(long);

    std::printf("%p\n", start);
    data = (void*) start;
  }

  long get_total_size() {
    return 2 * sizeof(long) + sizeof(query_type) + sizeof(bool) + data_size + 
      sizeof(void *);
  }

  zmq::message_t generate_request() {
    zmq::message_t request(get_total_size());
    long size = get_total_size();
    mempcpy((void *) request.data(), this, size); 
    return request;
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