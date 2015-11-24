#include "../deps/crow_all.h"

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/ping")
  .methods("GET"_method)
  ([] {
    return "pong";
  });

  CROW_ROUTE(app, "/table/<string>")
  .methods("GET"_method)
  ([](std::string table_name) {
    return crow::response(table_name);
  });

  app.port(8080).multithreaded().run();
}