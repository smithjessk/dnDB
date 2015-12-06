#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

#include "table_connection.h"
#include "table_worker.h"

class worker_manager {
 private:
  uint32_t queries_filed;
  std::unordered_map<std::string, table_worker*> workers;
  std::unordered_map<std::string, table_connection*> conns;

 public:
  worker_manager() {
    workers.reserve(10); 
    conns.reserve(10);
    queries_filed = 0;
  };

  void set_initial_size(int size) {
    workers.reserve(size);
    conns.reserve(size);
  }

  uint32_t get_next_query_id() {
    return queries_filed++;
  }

  void add(table_worker *tw, table_connection *tc) {
    workers[tw->get_table_name()] = tw;
    conns[tw->get_table_name()] = tc;
  }

  void delete_table(std::string name) {
    conns[name]->mark_deleted();
  }

  table_connection *get_conn(std::string name) {
    return conns.at(name);
  }

  bool table_exists(std::string name) {
    return workers.count(name) > 0 && conns.count(name) > 0;
  }
};

#endif // WORKER_MANAGER_H