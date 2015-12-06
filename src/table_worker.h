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

struct sql_query {
  std::string table_name;
  std::vector<std::string> column_names;

  sql_query(std::string table_name, std::vector<std::string> column_names) {
    this->table_name = table_name;
    this->column_names = column_names;
  }
};

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

  sql_query parse_sql_query(std::string c) {
    std::vector<std::string> sqlCommand;
    char delimiter1 = ' ';
    char delimiter2 = ',';
    std::string token1, token2;
    std::stringstream ss(c);
    while(getline(ss, token1, delimiter1)){
      if(token1.find(delimiter2)){
        std::stringstream ss2(token1);
        while(getline(ss2, token2, delimiter2)){
          sqlCommand.push_back(token2);
        }
      }else{
        sqlCommand.push_back(token1);
      }
    }
    //basic check for minimum amount of arguments
    if(sqlCommand.size() < 4){
      throw "Not enougn commands";
    }
    // check if select and from key words are there
    else if(c.find("SELECT") == -1 && c.find("FROM") == -1){
      throw "Couldn't find both SELECT and FROM";
    }
    else if(c.find("SELECT") > c.find("FROM")){
      throw "SELEECT after FROM";
    }
    else {
      std::string table_name = sqlCommand.at(sqlCommand.size() - 1);
      std::vector<std::string> col_names;
      for (size_t i = 1; i < sqlCommand.size() - 2; i++) {
        col_names.push_back(sqlCommand.at(i));
      }
      return sql_query(table_name, col_names);
    }
  }

  void perform_sql_query(query &q) {
    sql_query select_query = parse_sql_query(q.data);
    if (this->table_name != select_query.table_name) {
      throw "Table names don't match";
    }
    std::string result = "";
    for (size_t i = 0; i < select_query.column_names.size(); i++) {
      std::string c_name = select_query.column_names.at(i);
      std::vector<std::string> entries = table.getColumn(c_name);
      result += c_name;
      result += "\n";
      for (size_t j = 0; j < entries.size() - 1; j++) {
        result += entries.at(j) + ",";
      }
      result += entries.at(entries.size() - 1);
      result += "\n";
    }
    q.set_data(result);
    q.mark_successful();
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
        } catch (...) {
          std::printf("Got error in READ %s", table_name.c_str());
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
        } catch (...) {
          std::printf("Got error in UPDATE %s", table_name.c_str());
          q.mark_unsuccessful();
        }
        break;
      }
      case DELETE: {
        try {
          perform_delete(q);
        } catch (...) {
          std::printf("Got error in DELETE %s", table_name.c_str());
          q.mark_unsuccessful();
        }
        break;
      }
      case SQL: {
        try {
          perform_sql_query(q);
        } catch (...) {
          std::printf("Got error in SQL %s", table_name.c_str());
          q.mark_unsuccessful();
        }
        break;
      }
      case ADD_COLUMN: {
        try {
          table.addColumn("\"" + q.data + "\"");
          q.mark_successful();
          q.set_data("column added");
        } catch (...) {
          std::printf("Got error in ADD COLUMN: %s", table_name.c_str());
          q.mark_unsuccessful();
        }
      }
      case ADD_ROW: {
        try {
          table.addRow(q.data);
          q.mark_successful();
          q.set_data("row added");
        } catch (...) {
          std::printf("Got error in ADD ROW: %s", table_name.c_str());
          q.mark_unsuccessful();
        }
      }
      default: {
        std::printf("Got unknown query type on table %s\n", 
          table_name.c_str());
        q.mark_unsuccessful();
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
      input_socket = zmq::socket_t(context, ZMQ_REP);
      input_socket.bind(get_bound_address());
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