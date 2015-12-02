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
      long id = manager.get_next_query_id();
      /*query *q = new query(id, READ);
      q->data = "abc";*/
      test *t = new test();
      t->data[0] = 'a';
      zmq::message_t request = t->generate_message();
      /*std::cout << "generated message" << std::endl;
      char *temp = (char *)request.data() + sizeof(long) + sizeof(bool) + 
        sizeof(query_type);
      */
      std::unique_lock<std::mutex> lock(conn->mutex);
      conn->socket.send(request);
      zmq::message_t reply;
      delete t;
      conn->socket.recv(&reply);
      lock.unlock();
      std::cout << "Got reply" << std::endl;
      /*query *response = new query((char *) reply.data());
      std::printf("id = %lu\n", response->id);
      std::string data = response->data;
      std::printf("data = %s\n", data.c_str());*/
      test *response = new test((char *) reply.data());
      std::cout << "first char: " << response->data[0] << std::endl;
      delete response;
      return crow::response("asdas");
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
    std::string table_name = body["table_name"].s();
    std::ostringstream os;
    os << table_name;
    return crow::response{os.str()};
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
  declare_routes(app);
  app.port(8080).run();
}