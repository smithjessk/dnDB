#include <arpa/inet.h>

#include "config_reader.h"
#include "table_worker.h"
#include "worker_manager.h"
#include "../deps/crow_all.h"
#include "../deps/zmq.hpp"

worker_manager manager;

bool contains_quote(std::string s) {
  return s.find('"') != std::string::npos;
}

std::string build_update_string(std::string table_name, std::string col_name, 
  int row_id, std::string value) {
  std::string ret_value;
  std::stringstream ss;
  ss << "\"" << table_name << "\",";
  ss << "\"" << col_name << "\",";
  ss << row_id << ",";
  ss << "\"" << value << "\"";
  ss >> ret_value;
  return ret_value;
}

query *send_and_get_response(table_connection *conn, query *q) {
  zmq::message_t request = q->generate_message();
  std::unique_lock<std::mutex> lock(conn->mutex);
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
      return crow::response(400); // Bad request
    }
    std::string table_name = body["table_name"].s();
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
      return crow::response(400);
    }
    try {
      std::string table_name = body["table_name"].s();
      table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, READ);
      q->set_data(table_name);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400);
      }
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(400); // Bad request
    }
  });

  CROW_ROUTE(app, "/table/update")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400);
    }
    try {
      std::string table_name = body["table_name"].s();
      std::string col_name = body["col_name"].s();
      int row_id = body["row_id"].i();
      std::string value = body["value"].s();
      if (contains_quote(table_name) || contains_quote(col_name) || 
        contains_quote(value)) { // Test this!
        return crow::response(400);
      }
      table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, UPDATE);
      q->set_data(build_update_string(table_name, col_name, row_id, 
        value));
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400);
      }
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(400);
    }
  });

  CROW_ROUTE(app, "/table/delete")
  .methods("POST"_method) 
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400);
    }
    try {
      std::string table_name = body["table_name"].s();
      table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, DELETE);
      q->set_data(table_name);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400);
      }
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(400);
    }
  });

  CROW_ROUTE(app, "/table/sql")
  .methods("POST"_method)
  ([](const crow::request &req) {
    auto body = crow::json::load(req.body);
    if (!body) {
      return crow::response(400);
    }
    try {
      std::string statement = body["statement"].s();
      // struct stmt = parse(statement);
      /*table_connection *conn = manager.get_conn(table_name);
      uint32_t id = manager.get_next_query_id();
      query *q = new query(id, DELETE);
      q->set_data(table_name);
      query *response = send_and_get_response(conn, q);
      if (!response->successful) {
        return crow::response(400);
      }
      std::string data = response->data;
      delete response;*/
      return crow::response(statement);
    } catch (int n) {
      return crow::response(400);
    }
  });
}

int main(int argc, char** argv) {
  int port, number_initial_tables, initial_table_port;
  try {
    std::unordered_map<std::string, std::string> map = read(argv[1]);
    port = stoi(map.at("host_port"));
    number_initial_tables = stoi(map.at("number_initial_tables"));
    initial_table_port = stoi(map.at("initial_table_port"));
  } catch (int n) {
    std::cout << "Could not read config file. Using defaults" << std::endl;
    port = 8080;
    number_initial_tables = 10;
    initial_table_port = 5555;
  }
  manager.set_initial_size(number_initial_tables);
  crow::SimpleApp app;
  int socket_port = initial_table_port;
  table_worker tw1("one", socket_port);
  table_connection tc1(tw1.get_port());
  manager.add(&tw1, &tc1);
  socket_port = tw1.get_port() + 1;
  table_worker tw2("two", socket_port);
  table_connection tc2(socket_port);
  manager.add(&tw2, &tc2);
  declare_routes(app);
  app.port(port).multithreaded().run();
}