#include <arpa/inet.h>
#include <stdexcept>

#include "config_reader.h"
#include "table_worker.h"
#include "worker_manager.h"
#include "../deps/crow_all.h"
#include "../deps/zmq.hpp"

worker_manager manager;

void create_worker(std::string table_name){
  std::string path = manager.get_data_directory() + table_name + ".csv";
  uint32_t port = manager.get_next_port();
  bool bound = false;
  table_worker *tw = NULL;
  while (!bound) {
    try {
      tw = new table_worker(path, port);
      bound = true;
    } catch (...) {
      std::printf("Port %d in use. Trying next one...\n", port);
      port = manager.get_next_port();
    }
  }
  if (tw == NULL) {
    throw "Could not create table worker";
  }
  table_connection *tc = new table_connection(tw->get_port());
  manager.add(tw, tc);
}

/**
 * Return a string that is used in table_worker.h to update a value in the 
 * table
 * @param  col_name   Column name to update
 * @param  row_id     Row ID to update
 * @param  value      Value to update to
 * @return            E.g. "my_col",1,"new_val"
 */
std::string build_update_string(std::string col_name, int row_id,
  std::string value) {
  std::string ret_value;
  std::stringstream ss;
  ss << "\"" << col_name << "\",";
  ss << row_id << ",";
  ss << "\"" << value << "\"";
  ss >> ret_value;
  return ret_value;
}

query *send_and_get_response(table_connection *conn, query *q) {
  zmq::message_t request = q->generate_message();
  std::unique_lock<std::mutex> lock(conn->mutex);
  if (conn->table_deleted) {
    q->mark_unsuccessful();
    q->set_data("could not perform request; table deleted");
    return q;
  }
  conn->socket.send(request);
  delete q;
  zmq::message_t reply;
  conn->socket.recv(&reply);
  lock.unlock();
  return new query((char *) reply.data());
}

void create_file(std::string file_name) {
  std::ofstream new_table;
  new_table.open(file_name);
  new_table.close();
}

void declare_routes(crow::SimpleApp &app) {
  CROW_ROUTE(app, "/ping")
  .methods("GET"_method)
  ([] {
    return "pong";
  });

  CROW_ROUTE(app, "/table/create")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400, "no body found"); // Bad request
    }
    std::string table_name = body["table_name"].s();
    if (!manager.table_exists(table_name)) {
      return crow::response(400, "table not found");
    }
    try {
      create_file(manager.get_data_directory() + table_name + ".csv");
      create_worker(table_name);
    } catch (std::string s) {
      return crow::response(500);
    } catch (int n) {
      return crow::response(500);
    }
    return crow::response(table_name);
  });

  CROW_ROUTE(app, "/table/read")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400, "no body found"); // Bad request
    }
    try {
      std::string table_name = body["table_name"].s();
      if (!manager.table_exists(table_name)) {
        return crow::response(400, "table not found");
      }
      table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, READ);
      q->set_data(table_name);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400, response->data);
      }
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(500);
    }
  });

  CROW_ROUTE(app, "/table/update")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400, "no body found"); // Bad request
    }
    try {
      std::string table_name = body["table_name"].s();
      if (!manager.table_exists(table_name)) {
        return crow::response(400, "table not found");
      }
      std::string col_name = body["col_name"].s();
      int row_id = body["row_id"].i();
      std::string value = body["value"].s();
      table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, UPDATE);
      q->set_data(build_update_string(col_name, row_id, value));
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400, response->data);
      }
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(500);
    }
  });

  CROW_ROUTE(app, "/table/delete")
  .methods("POST"_method) 
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400, "no body found"); // Bad request
    }
    try {
      std::string table_name = body["table_name"].s();
      if (!manager.table_exists(table_name)) {
        return crow::response(400, "table not found");
      }
      table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, DELETE);
      q->set_data(table_name);
      zmq::message_t request = q->generate_message();
      std::unique_lock<std::mutex> lock(conn->mutex);
      conn->socket.send(request);
      delete q;
      zmq::message_t reply;
      conn->socket.recv(&reply);
      query *response = new query((char *) reply.data());
      if (!response->successful) {
        lock.unlock();
        return crow::response(400, q->data);
      }
      manager.delete_table(table_name);
      lock.unlock();
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(500);
    }
  });

  CROW_ROUTE(app, "/table/sql")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400, "no body found"); // Bad request
    }
    try {
      std::string table_name = body["table_name"].s();
      std::string statement = body["statement"].s();
      if (!manager.table_exists(table_name)) {
        return crow::response(400, "table not found");
      }
      table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, DELETE);
      q->set_data(table_name);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400, response->data);
      }
      std::string data = response->data;
      delete response;
      return crow::response(statement);
    } catch (int n) {
      return crow::response(500);
    }
  });

  CROW_ROUTE(app, "/table/add_column")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body){
      return crow::response(400);
    }
    try {
      std::string table_name = body["table_name"].s();
      if (!manager.table_exists(table_name)) {
        return crow::response(400, "table not found");
      }
      std::string col_name = body["col_name"].s();
      uint32_t id = manager.get_next_query_id();
      table_connection *conn = manager.get_conn(table_name);
      query *q = new query(id, ADD_COLUMN);
      q->set_data(col_name);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400, response->data);
      }
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(500);
    }
  });


  CROW_ROUTE(app, "/table/add_row")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body){
      return crow::response(400);
    }
    try {
      std::string table_name = body["table_name"].s();
      if (!manager.table_exists(table_name)) {
        return crow::response(400, "table not found");
      }
      std::string row = body["row"].s();
      uint32_t id = manager.get_next_query_id();
      table_connection *conn = manager.get_conn(table_name);
      query *q = new query(id, ADD_ROW);
      q->set_data(row);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400, response->data);
      }
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(500);
    }
  });
}

int main(int argc, char** argv) {
  int port, number_initial_tables, initial_table_port;
  std::string path_to_directory;
  try {
    if (argc >= 2 && std::ifstream(argv[1])){
      std::unordered_map<std::string, std::string> config_map = read(argv[1]);
      port = stoi(config_map.at("host_port"));
      number_initial_tables = stoi(config_map.at("number_initial_tables"));
      initial_table_port = stoi(config_map.at("initial_table_port"));
      path_to_directory = config_map.at("path_to_directory");
      if (path_to_directory.substr(path_to_directory.length()-1, 1) != "/") {
        path_to_directory += "/";
      }
    } else {
      std::cout << "Could not read config file. Using defaults" << std::endl;
      port = 8080;
      number_initial_tables = 10;
      initial_table_port = 5555;
      path_to_directory = "./data/";
    }
  } catch (std::exception e) {
    std::cout << "Could not read config file. Using defaults" << std::endl;
    port = 8080;
    number_initial_tables = 10;
    initial_table_port = 5555;
    path_to_directory = "./data/";
  }
  manager.set_initial_size(number_initial_tables);
  manager.set_data_directory(path_to_directory);
  manager.set_initial_port(initial_table_port);
  crow::SimpleApp app;
  create_worker("sample");
  declare_routes(app);
  bool connected = false;
  while (!connected) {
    try {
      app.port(port).multithreaded().run();
      connected = true;
    } catch (...) {
      std::printf("Port %d in use. Trying next one...\n", port);
      port++;
    }
  }
}