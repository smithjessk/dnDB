#include "table_worker.h"
#include "worker_manager.h"
#include "../deps/crow_all.h"

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
    std::ostringstream os;
    os << table_name;
    return crow::response{os.str()};
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
      long query_id = manager.add_query(table_name, READ);
      query *q = manager.get_query_pointer(query_id);
      while (!q->finished) {
        std::unique_lock<std::mutex> query_lock(q->mutex);
        q->cv.wait(query_lock);
      }
      std::string *data = (std::string *) q->data;
      return crow::response("POST /table/read " + *data);
    } catch (int n) {
      std::printf("POST /table/read encountered an error: %d\n", n);
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
  table_worker tw1("one");
  manager.add_worker(&tw1);
  declare_routes(app);
  app.port(8080).run();
}