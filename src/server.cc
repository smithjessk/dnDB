#include <arpa/inet.h>

#include "table_worker.h"
#include "worker_manager.h"
#include "../deps/crow_all.h"
#include "../deps/zmq.hpp"

/**
 * 1) Extract information from the query
 * 2) Send this information into the appropriate table worker
 * 3) Once the table worker is done (or 15 seconds have passed), reply using 
 * the appropriate query object 
 */

// To properly hang up on results, maintain a map between some sort of query 
// ID and the query struct. Can't use queue because that would introduce race 
// conditions. 
// Remove the element from the map in the HTTP response code.

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
    return crow::response{table_name};
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
      zmq::message_t request = q->generate_message();
      std::unique_lock<std::mutex> lock(conn->mutex);
      conn->socket.send(request);
      delete q;
      zmq::message_t reply;
      conn->socket.recv(&reply);
      lock.unlock();
      query *response = new query((char *) reply.data());
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
      zmq::message_t request = q->generate_message();
      std::unique_lock<std::mutex> lock(conn->mutex);
      conn->socket.send(request);
      delete q;
      zmq::message_t reply;
      conn->socket.recv(&reply);
      lock.unlock();
      query *response = new query((char *) reply.data());
      std::string data = response->data;
      delete response;
      return crow::response(data);
    } catch (int n) {
      return crow::response(400);
    }
  });

  CROW_ROUTE(app, "/table/delete")
  .methods("DELETE"_method) 
  ([](const crow::request &req) {
    return crow::response("DELETE /table/delete");
  });
}

int main() {
  crow::SimpleApp app;
  table_worker tw1("one", 5555);
  table_connection tc1(5555);
  manager.add(&tw1, &tc1);
  table_worker tw2("two", 5556);
  table_connection tc2(5556);
  manager.add(&tw2, &tc2);
  declare_routes(app);
  app.port(8080).multithreaded().run();
}