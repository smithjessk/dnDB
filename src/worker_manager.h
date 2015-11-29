#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

#include "query_types.h"

class worker_manager {
 private:
  std::unordered_map<std::string, table_worker*> workers;
  std::unordered_map<long, query_response*> queries;

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

#endif // WORKER_MANAGER_H