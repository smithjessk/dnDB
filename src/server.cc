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

worker_manager wm;

void declare_routes(crow::SimpleApp &app) {
  CROW_ROUTE(app, "/a")
  .methods("GET"_method)
  ([] {
    std::mutex *copy = wm.get_queue_mutex("one");
    {
      std::cout << "route vying for queue_mutex" << std::endl;
      std::lock_guard<std::mutex> lock(*copy);
      std::cout << "route acquired queue_mutex" << std::endl;
      wm.notify("one");
    }
    return "malicious";
  });


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
    return crow::response("POST /table/read");
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
  wm.add_worker(&tw1);
  declare_routes(app);
  app.port(8080).run();
}