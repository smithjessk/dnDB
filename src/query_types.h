#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

enum query_type {CREATE, READ, UPDATE, DELETE};

struct query {
  long id; // Same as the query_response id
  query_type type;
  std::mutex mutex;
  std::condition_variable cv;
  bool finished;
  bool successful;
  void *data;

  query(long id, query_type qt) {
    this->id = id;
    this->type = qt;
    this->finished = false;
    this->successful = false;
    this->data = NULL;
  }
};

struct query_response {
  long id;
  bool successful;
  void *data;

  query_response(long id) {
    this->id = id;
    successful = false;
    data = NULL;
  }
};

#endif // QUERY_TYPES_H