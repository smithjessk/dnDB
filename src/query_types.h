#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

enum query_type {CREATE, READ, UPDATE, DELETE};

struct query {
  long id; // Same as the query_response id
  query_type type;
};

struct query_response {
  long id;
  bool successful;
  void *data;
};

#endif // QUERY_TYPES_H