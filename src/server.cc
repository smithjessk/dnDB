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
  std::thread t;
  std::mutex queue_mutex;
  std::condition_variable cv;
  std::string table_name;
  std::queue<query> queue;
  bool done;
  bool notified;
  
  // Used to notify about new messages. call cv.notify_one() when you push a 
  // new message. 
  // What happens if this is processing messages in the queue? 
  // I think it won't do anything since there would be no threads waiting on 
  // the condition variable.
  //  
  // TODO: Handle exceptions. Note that if wait exits via exception, 
  // process_messages still acquires the lock.

  void process(query q) {};

  void process_messages() {
    while (!done) {
      std::cout << "table_worker vying for queue_mutex" << std::endl;
      std::unique_lock<std::mutex> lock(queue_mutex);
      std::cout << "table_worker acquired queue_mutex" << std::endl;
      while (!notified) { // To avoid spurious wakeups
        std::cout << "Waiting on cv..." << std::endl;
        cv.wait(lock); // Note that this releases the lock
      }
      while (!queue.empty()) {
        std::cout << "Processing queue..." << std::endl;
      }
      std::cout << "Done processing" << std::endl;
      notified = false;
    }
  };

 public:
  table_worker(std::string name)
    : table_name(name),
    done(false),
    notified(false),
    t([this] { process_messages(); }) {}

  std::string get_name() {
    return table_name;
  }

  std::condition_variable *get_cv() {
    return &cv;
  }

  std::mutex *get_queue_mutex() {
    return &queue_mutex;
  }

  void notify() {
    this->notified = true;
    this->cv.notify_one();
  }

  std::thread spawn() {
    return std::thread( [this] { process_messages(); });
  }
};

class worker_manager {
 private:
  std::unordered_map<std::string, table_worker*> workers;
 public:
  worker_manager() {
    workers.reserve(10); // Decent initial guess on number of tables
  };

  void add_worker(table_worker *tw) {
    workers[tw->get_name()] = tw;
  }

  std::mutex *get_queue_mutex(std::string worker_name) {
    try {
      return workers.at(worker_name)->get_queue_mutex();
    } catch (int n) { // In case the worker_name is not valid
      return NULL;
    }
  }

  void notify(std::string worker_name) {
    try {
      workers.at(worker_name)->notify();
    } catch (int n) { // In case the worker_name is not valid
      std::printf("Tried to notify %s but could not find it\n", 
        worker_name.c_str());
      return;
    }
  }
};

worker_manager wm;

// To properly hang up on results, maintain a map between some sort of query 
// ID and the query struct. Can't use queue because that would introduce race 
// conditions. 
// Remove the element from the map in the HTTP response code.
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