#ifndef TABLE_WORKER_H
#define TABLE_WORKER_H

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <thread>
#include <arpa/inet.h>

#include "../deps/zmq.hpp"
#include "query_types.h"

class table_worker {
 private:
  std::string table_name;
  bool done;
  zmq::context_t context;
  zmq::socket_t input_socket;
  std::thread t;
  std::string protocol;
  int port;
  bool connected;

  void process(query &q) {
    switch (q.type) {
      case READ: {
        q.set_data("READ TABLE " + q.data);
        q.mark_successful();
        break;
      }
      case UPDATE: {
        q.set_data("UPDATE TABLE " + q.data);
        q.mark_successful();
        break;
      }
      case DELETE: {
        q.set_data("DELETE TABLE " + q.data);
        q.mark_successful();
        break;
      }
      case SQL: {
        q.set_data("SQL TABLE " + q.data);
        q.mark_successful();
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
      process(q);
      zmq::message_t reply = q.generate_message();
      input_socket.send(reply);
    }
  };

 public:
  table_worker(std::string name, int port)
    : table_name(name),
    port(port),
    protocol("tcp"),
    connected(false),
    done(false),
    context(1),
    input_socket(context, ZMQ_REP) { // Compiler wants this
      while (!connected) {
        try {
          input_socket.bind(get_bound_address());
          connected = true; // If it gets here, we're connected  
        } catch (int n) {
          std::printf("%s in use. Trying next port...\n", 
            get_bound_address().c_str());
        }
      }
      std::printf("Successfully bound to address %s\n", 
        get_bound_address().c_str());
      t = std::thread([this] { process_messages(); });
  }

  int get_port() {
    return port;
  }

  std::string get_table_name() {
    return table_name;
  }

  std::string get_protocol() {
    return protocol;
  }

  std::string get_bound_address() {
    std::string address;
    std::stringstream ss;
    ss << get_protocol();
    ss << "://*:";
    ss << port;
    ss >> address;
    return address;
  }
};

#endif // TABLE_WORKER_H