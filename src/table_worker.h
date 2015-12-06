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
#include "table.h"

class table_worker {
 private:
  std::string file_path;
  std::string table_name;
  bool done;
  bool deleted_table;
  bool processing_request;
  zmq::context_t context;
  zmq::socket_t input_socket;
  std::thread t;
  std::string protocol;
  int port;
  bool connected;
  Table table;

  void perform_delete(query &q) {
    if (remove(file_path.c_str()) != 0) {
      std::printf("Got error in DELETE %s: Could not remove file", 
      table_name.c_str());
      q.mark_unsuccessful();
    } else {
      deleted_table = true;
      q.set_data("table deleted");
      q.mark_successful();
    }
  }

  /**
   * Processes a given query, saving the table as necessary. If an error is 
   * thrown here, it is caught and then the query is marked as not successful.
   * @param q [description]
   */
  void process(query &q) {
    switch (q.type) {
      case READ: {
        try {
          q.set_data(table.getSerializedTable());
          q.mark_successful();
        } catch (int n) {
          std::printf("Got error in READ %s: %d", table_name.c_str(), n);
          q.mark_unsuccessful();
        }
        break;
      }
      case UPDATE: {
        try {
          std::vector<std::string> parts = split(q.data);
          std::string col_name = parts.at(0);
          int row_id = stoi(parts.at(1));
          std::string value = parts.at(2);
          table.setElement(row_id, col_name, value);
          q.set_data("table updated");
          q.mark_successful();
        } catch (int n) {
          std::printf("Got error in UPDATE %s: %d", table_name.c_str(), n);
          q.mark_unsuccessful();
        }
        break;
      }
      case DELETE: {
        try {
          perform_delete(q);
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
      case ADD_COLUMN: {
        try {
          table.addColumn("\"" + q.data + "\"");
          q.mark_successful();
          q.set_data("column added");
        } catch (int n) {
          std::printf("Got error in ADD COLUMN: %s: %d", table_name.c_str(), 
            n);
          q.mark_unsuccessful();
        }
      }
      case ADD_ROW: {
        try {
          table.addRow(q.data);
          q.mark_successful();
          q.set_data("row added");
        } catch (int n) {
          std::printf("Got error in ADD ROW: %s: %d", table_name.c_str(), 
            n);
          q.mark_unsuccessful();
        }
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
      if (deleted_table) {
        q.mark_unsuccessful();
        q.set_data("could not perform request; table deleted");
      } else {
        processing_request = true;
        process(q);
        processing_request = false;  
      }
      zmq::message_t reply = q.generate_message();
      input_socket.send(reply);
    }
  };

 public:
  /**
   * Constructs this table_worker using the given information. Communicates 
   * using the TCP protocol. Once this worker has connected to the given port,
   * spawns the worker thread to start processing messages.
   * @param file_path The path of the file that drives this table.
   * @param port The port to first try to bind on. If this port cannot be 
   * connected to, then increment it and try again.
   */
  table_worker(std::string file_path, int port)
    : file_path(file_path),
    done(false),
    deleted_table(false),
    processing_request(false),
    context(1),
    input_socket(context, ZMQ_REP),
    protocol("tcp"),
    port(port),
    connected(false),
    table(file_path) {
      table_name = table.getTableName();
      while (!connected) {
        try {
          input_socket = zmq::socket_t(context, ZMQ_REP);
          input_socket.bind(get_bound_address());
          connected = true;
        } catch (int n) {
          std::printf("Got error %d\n", n);
          port++;
        } catch (zmq::error_t error) {
          std::printf("%s in use. Trying next port...\n", 
            get_bound_address().c_str());
          port++;
          std::printf("next port is %d\n", port);
          std::printf("%s\n", get_bound_address().c_str());
        }
      }
      std::printf("Successfully bound to address %s\n", 
        get_bound_address().c_str());
      t = std::thread([this] { process_messages(); });
  }

  /**
   * Associated with the atexit() of this main thread. Marks that no more
   * requests should be processed and then, once the final request is done,
   * dies with grace. 
   */
  ~table_worker() {
    done = true;
    while(processing_request) {}
    if (!deleted_table) {
      table.saveTable(file_path);
    }
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
    ss.clear();
    return address;
  }
};

#endif // TABLE_WORKER_H