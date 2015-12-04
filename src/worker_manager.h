#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

#include "query_types.h"

class worker_manager {
 private:
  uint32_t queries_filed;
  int next_port;
  std::unordered_map<std::string, table_worker*> workers;
  std::unordered_map<std::string, table_connection*> conns;

 public:
  worker_manager() {
    workers.reserve(10); 
    conns.reserve(10);
    queries_filed = 0;
    next_port = 5555;
  };

  void set_port(int port) {
    next_port = port;
  }

  void set_initial_size(int size) {
    workers.reserve(size);
    conns.reserve(size);
  }

  uint32_t get_next_query_id() {
    return queries_filed++;
  }

  int get_next_port() {
    return next_port++;
  }

  void add(table_worker *tw, table_connection *tc) {
    workers[tw->get_table_name()] = tw;
    conns[tw->get_table_name()] = tc;
  }

  table_connection *get_conn(std::string name) {
    return conns.at(name);
  }
};

#endif // WORKER_MANAGER_H