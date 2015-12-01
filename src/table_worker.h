#ifndef TABLE_WORKER_H
#define TABLE_WORKER_H

#include <string>
#include <cstring>
#include <sstream>
#include <thread>

#include "../deps/zmq.hpp"
#include "query_types.h"

class table_worker {
 private:
  std::string table_name;
  bool done;
  zmq::context_t context;
  zmq::socket_t input_socket;
  std::thread t;
  int port;

  void process(query *q) {
    switch (q->type) {
      case READ: {
        char *result = (char*) malloc(32 * sizeof(char));
        std::string id_as_string;
        std::stringstream ss;
        ss << q->id;
        ss >> id_as_string;
        strcpy(result, id_as_string.c_str());
        q->data = result;
        break;
      }
      default: {
        break;
      }
    }
  };

  void process_messages() {
    while (!done) {
      zmq::message_t request;
      input_socket.recv(&request);
      query q((char *) request.data());
      // process(q);
      zmq::message_t reply(5);
      memcpy ((void *) reply.data (), "World", 5);
      input_socket.send(reply);
    }
  };

 public:
  table_worker(std::string name, int port)
    : table_name(name),
    port(port),
    done(false),
    context(1),
    input_socket(context, ZMQ_REP) { // Compiler wants this
      input_socket.bind(get_bound_address());
      t = std::thread([this] { process_messages(); });
  }

  std::string get_table_name() {
    return table_name;
  }

  std::string get_bound_address() {
    std::string address;
    std::stringstream ss;
    ss << "tcp://*:";
    ss << port;
    ss >> address;
    return address;
  }
};

#endif // TABLE_WORKER_H