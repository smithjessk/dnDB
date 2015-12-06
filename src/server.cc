#include <arpa/inet.h>
#include <stdexcept>

#include "config_reader.h"
#include "table_worker.h"
#include "worker_manager.h"
#include "../deps/crow_all.h"
#include "../deps/zmq.hpp"

worker_manager manager;

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
    uint32_t id = manager.get_next_query_id();
    query *q = new query(id, CREATE);
    q->set_data(table_name);
    // manager.create_table(q);
    // create_worker(q);
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
      // struct stmt = parse(statement);
      if (!manager.table_exists(table_name)) {
        return crow::response(400, "table not found");
      }
      /*table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, DELETE);
      q->set_data(table_name);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400, response->data);
      }
      std::string data = response->data;
      delete response;*/
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
  try {
    if(argc >= 2 && std::ifstream(argv[1])){
      std::unordered_map<std::string, std::string> config_map = read(argv[1]);
      port = stoi(config_map.at("host_port"));
      number_initial_tables = stoi(config_map.at("number_initial_tables"));
      initial_table_port = stoi(config_map.at("initial_table_port"));
    } else {
      std::cout << "Could not read config file. Using defaults" << std::endl;
      port = 8080;
      number_initial_tables = 10;
      initial_table_port = 5555;
    }
  } catch (std::exception e) {
    std::cout << "Could not read config file. Using defaults" << std::endl;
    port = 8080;
    number_initial_tables = 10;
    initial_table_port = 5555;
  }
  manager.set_initial_size(number_initial_tables);
  crow::SimpleApp app;
  int socket_port = initial_table_port;
  table_worker tw1("./data/sample.csv", socket_port);
  table_connection tc1(tw1.get_port());
  manager.add(&tw1, &tc1);
  declare_routes(app);
  app.port(port).multithreaded().run();
}