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
  std::mutex queue_mutex;
  std::condition_variable cv;
 public:
  bool done;
  bool notified;
 private:
  std::string table_name;
  std::queue<query> queue;/*
  std::mutex queue_mutex; // Used for sync. on pushing/popping from the queue.
  std::mutex waiting_mutex; // Used for synchronization on cv.
  std::condition_variable cv; */
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
      std::cout << "not done, trying to get queue_lock" << std::endl;
      std::unique_lock<std::mutex> lock(queue_mutex);
      std::cout << "Acquired queue_mutex" << std::endl;
      while (!notified) { // To avoid spurious wakeups
        std::cout << "Waiting on cv..." << std::endl;
        cv.wait(lock);
      }
      while (!queue.empty()) {
        std::cout << "Processing thing in the queue" << std::endl;
      }
      std::cout << "Done processing" << std::endl;
      notified = false;
    }
  };

  /*
  void process_messages() {
    while (true) {
      std::cout << "process_messages wants waiting_mutex" << std::endl;
      std::unique_lock<std::mutex> waiting_lock(waiting_mutex);
      cv.wait(waiting_lock); // Predicate to wait for non-empty queue?
      std::cout << "process_messages acquired waiting_lock" << std::endl;
      query q;
      while (!queue.empty()) {
        { // to avoid popping and pushing at the same time
          std::cout << "process_messages wants to acquire queue_mutex" << std::endl;
            std::lock_guard<std::mutex> lock(queue_mutex);
          std::cout << "process_messages acquired queue_mutex" << std::endl;
          q = queue.front();
          queue.pop();
        }
        process(q);
      }
    }
  }*/

 public:
  table_worker(std::string name)
    : table_name(name),
    done(false),
    notified(false) {}

  std::string get_name() {
    return table_name;
  }

  std::condition_variable *get_cv() {
    return &cv;
  }

  std::mutex *get_queue_mutex() {
    return &queue_mutex;
  }

  std::thread spawn() {
    return std::thread( [this] { process_messages(); });
  }
};

std::unordered_map<std::string, table_worker*> workers(0);
table_worker tw1("one");

void declare_routes(crow::SimpleApp &app) {
  CROW_ROUTE(app, "/a")
  .methods("GET"_method)
  ([] {
    std::thread t1 = workers.at("one")->spawn();
    std::mutex *copy = workers.at("one")->get_queue_mutex();
    {
      std::lock_guard<std::mutex> lock(*copy);
      workers.at("one")->notified = true;
      workers.at("one")->get_cv()->notify_one();  
    }
    std::cout << "notified" << std::endl;
    t1.join();
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
  workers["one"] = &tw1;
  declare_routes(app);
  app.port(8080).run();
}