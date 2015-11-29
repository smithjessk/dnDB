#ifndef TABLE_WORKER_H
#define TABLE_WORKER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <queue>

#include "query_types.h"

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
  //  
  // TODO: Handle exceptions. Note that if wait exits via exception, 
  // process_messages still acquires the lock.

  void process(query q) {};

  void process_messages() {
    while (!done) {
      std::unique_lock<std::mutex> lock(queue_mutex);
      while (!notified) { // To avoid spurious wakeups
        cv.wait(lock); // Note that this releases the lock
      }
      while (!queue.empty()) {
        // process(q);
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

  void notify() {
    this->notified = true;
    this->cv.notify_one();
  }

  std::thread spawn() {
    return std::thread( [this] { process_messages(); });
  }
};

#endif // TABLE_WORKER_H