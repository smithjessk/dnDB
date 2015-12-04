#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

#include "query_types.h"

class worker_manager {
 private:
  uint32_t queries_filed;
  std::unordered_map<std::string, table_worker*> workers;
  std::unordered_map<std::string, table_connection*> conns;

 public:
  worker_manager() {
    workers.reserve(10); // Decent initial guess on number of tables
    conns.reserve(10);
    queries_filed = 0;
  };

  uint32_t get_next_query_id() {
    return queries_filed++;
  }

  void add(table_worker *tw, table_connection *tc) {
    workers[tw->get_table_name()] = tw;
    conns[tw->get_table_name()] = tc;
  }

  table_connection *get_conn(std::string name) {
    return conns[name];
  }
};

#endif // WORKER_MANAGER_H