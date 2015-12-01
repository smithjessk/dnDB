#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

#include "query_types.h"

class worker_manager {
 private:
  std::unordered_map<std::string, table_worker*> workers;
  std::unordered_map<long, query*> queries;
  long queries_filed;

 public:
  worker_manager() {
    workers.reserve(10); // Decent initial guess on number of tables
    queries_filed = 0;
  };

  long get_next_query_id() {
    return queries_filed++;
  }

  void add_worker(table_worker *tw) {
    workers[tw->get_table_name()] = tw;
  }

  // Return the query_id
  long add_query(std::string table_name, query_type type) {
    query q(get_next_query_id(), type);
    queries[q.id] = &q;
    return queries[q.id]->id;
  }

  query *get_query_pointer(long id) {
    return queries[id];
  }
};

#endif // WORKER_MANAGER_H