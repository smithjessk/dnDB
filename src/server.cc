#include "../deps/crow_all.h"

int main() {
  crow::SimpleApp app;

  CROW_ROUTE(app, "/ping")
  ([] {
    return "pong";
  });

  CROW_ROUTE(app, "/table/<string>")
  ([](std::string table_name) {
    return crow::response(table_name);
  });

  app.port(8080).multithreaded().run();
}