#ifndef TABLE_WORKER_H
#define TABLE_WORKER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cstring>
#include <sstream>
#include <queue>

#include "query_types.h"

class table_worker {
 private:
  std::thread t;
  std::mutex queue_mutex;
  std::condition_variable cv;
  std::string table_name;
  std::queue<query*> queue;
  bool done;
  bool notified;
  
  // Used to notify about new messages. call cv.notify_one() when you push a 
  // new message. 
  //  
  // TODO: Handle exceptions. Note that if wait exits via exception, 
  // process_messages still acquires the lock.

  void process(query *q) {
    switch (q->type) {
      case READ: {
        std::printf("Handling read query\n");
        char *result = (char*) malloc(32 * sizeof(char));
        std::string id_as_string;
        std::stringstream ss;
        ss << q->id;
        ss >> id_as_string;
        strcpy(result, id_as_string.c_str());
        q->data = result;
        break;
      }
      default: {
        break;
      }
    }
  };

  void process_messages() {
    while (!done) {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      while (!notified) { // To avoid spurious wakeups
        cv.wait(queue_lock); // Note that this releases the queue_lock
      }
      while (!queue.empty()) {
        query *q = queue.front();
        queue.pop();
        process(q);
        q->finished = true;
        q->cv.notify_one();
      }
      notified = false;
    }
  };

 public:
  table_worker(std::string name) {
    table_name = name;
    done = false;
    notified = false;
    t = std::thread([this] { process_messages(); });
  }

  std::string get_name() {
    return table_name;
  }

  std::condition_variable *get_cv() {
    return &cv;
  }

  std::mutex *get_queue_mutex() {
    return &queue_mutex;
  }

  void add_query(query *q) {
    queue.push(q);
  }

  void notify() {
    this->notified = true;
    this->cv.notify_one();
  }
};

#endif // TABLE_WORKER_H