#include <mutex>
#include <condition_variable>
#include <string>
#include <map>
#include <queue>

#include "../deps/crow_all.h"

/**
 * 1) Extract information from the query
 * 2) Send this information into the appropriate table worker
 * 3) Once the table worker is done (or 15 seconds have passed), reply using 
 * the appropriate query object 
 */

struct query {

};

struct query_response {
  bool successful;
  void *data;
};

class table_worker {
 private:
  std::string table_name;
  std::queue<query> queue;
  std::mutex queue_mutex;
  std::mutex waiting_mutex;
  std::condition_variable cv; // Used to notify about new messages

 void process(query q) {};

 void process_messages() {
  while (true) {
    std::unique_lock<std::mutex> waiting_lock(waiting_mutex);
    cv.wait(waiting_lock); // should we wait to see if the queue is empty?
    query q;
    while (!queue.empty()) {
      { // to avoid popping and pushing at the same time
        std::lock_guard<std::mutex> lock(queue_mutex);
        q = queue.front();
        queue.pop();
      }
      process(q);
    }  
  }
}

 public:
  table_worker(std::string name)
    : table_name(name) {}

  std::string get_name() {
    return table_name;
  }

  std::condition_variable *get_cv() {
    return &cv;
  }
};

std::unordered_map<std::string, table_worker*> workers;
table_worker tw1("one");

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
  declare_routes(app);
  app.port(8080).run();
}