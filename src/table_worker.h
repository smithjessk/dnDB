#ifndef TABLE_WORKER_H
#define TABLE_WORKER_H

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <thread>
#include <arpa/inet.h>
 #include <stdlib.h>

#include "../deps/zmq.hpp"
#include "query_types.h"
// #include "table.h"

class table_worker {
 private:
  std::string table_name;
  bool done;
  bool processing_request;
  zmq::context_t context;
  zmq::socket_t input_socket;
  std::thread t;
  std::string protocol;
  int port;
  bool connected;
  // table t;

  /**
   * Processes a given query, saving the table as necessary. If an error is 
   * thrown here, it is caught and then the query is marked as not successful.
   * @param q [description]
   */
  void process(query &q) {
    switch (q.type) {
      case READ: {
        try {
          /*
            
           */
          q.set_data("READ TABLE " + q.data);
          q.mark_successful();
        } catch (int n) {
          std::printf("Got error in READ %s: %d", table_name.c_str(), n);
          q.mark_unsuccessful();
        }
        break;
      }
      case UPDATE: {
        try {
          q.set_data("UPDATE TABLE " + q.data);
          q.mark_successful();
        } catch (int n) {
          std::printf("Got error in UPDATE %s: %d", table_name.c_str(), n);
          q.mark_unsuccessful();
        }
        break;
      }
      case DELETE: {
        try {
          q.set_data("DELETE TABLE " + q.data);
          q.mark_successful();
        } catch (int n) {
          std::printf("Got error in DELETE %s: %d", table_name.c_str(), n);
          q.mark_unsuccessful();
        }
        break;
      }
      case SQL: {
        try {
          q.set_data("SQL TABLE " + q.data);
          q.mark_successful();
        } catch (int n) {
          std::printf("Got error in SQL %s: %d", table_name.c_str(), n);
          q.mark_unsuccessful();
        }
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
      processing_request = true;
      process(q);
      processing_request = false;
      zmq::message_t reply = q.generate_message();
      input_socket.send(reply);
    }
  };

  /**
   * Associated with the atexit() of this main thread. Marks that no more
   * requests should be processed and then, once the final request is done,
   * dies with grace. 
   */
  void exit_gracefully() {
    done = true;
    while(processing_request) {} 
    // save_table(); 
  }

 public:
  /**
   * Constructs this table_worker using the given information. Communicates 
   * using the TCP protocol. Once this worker has connected to the given port,
   * spawns the worker thread to start processing messages.
   * @param name The name of the table
   * @param port The port to first try to bind on. If this port cannot be 
   * connected to, then increment it and try again.
   */
  // table_worker(std::string file_name, int port) {
  table_worker(std::string name, int port)
    : table_name(name),
    port(port),
    protocol("tcp"),
    connected(false),
    done(false),
    processing_request(false),
    context(1),
    // table(file_name)
    input_socket(context, ZMQ_REP) {
      while (!connected) {
        try {
          input_socket.bind(get_bound_address());
          connected = true;
        } catch (int n) {
          std::printf("%s in use. Trying next port...\n", 
            get_bound_address().c_str());
          port++;
        }
      }
      std::printf("Successfully bound to address %s\n", 
        get_bound_address().c_str());
      t = std::thread([this] { process_messages(); });
      // atexit(exit_gracefully);
  }

  /**
   * Return the port that this table worker uses to communicate. Used 
   * internally in get_bound_address()
   * @return For example, 5555
   */
  int get_port() {
    return port;
  }

  /**
   * Return the name of the table. Note that this is used in the name of the 
   * saved file.
   * @return For example, my_table
   */
  std::string get_table_name() {
    return table_name;
  }

  /**
   * Return the protocol that this table uses to communicate (e.g. tcp). Used
   * internally in get_bound_address()
   * @return For example, tcp
   */
  std::string get_protocol() {
    return protocol;
  }

  /**
   * Return the full address that this port is bound on. This port is used to
   * send and receive messages that contain queries for this table. 
   * @return For example, tcp://*:5555
   */
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