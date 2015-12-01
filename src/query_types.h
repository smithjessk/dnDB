#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

enum query_type {CREATE, READ, UPDATE, DELETE};

struct query {
  long id;
  query_type type;
  bool successful;
  void *data;

  query (long id, query_type qt) {
    this->id = id;
    this->type = qt;
    this->successful = false;
    this->data = NULL;
  }

  query (char *start) {
    id = *((long*) start);
    start += sizeof(long);

    type = *((query_type*) start);
    start += sizeof(query_type);

    successful = *((bool*) start);
    start += sizeof(bool);

    data = (void*) start;
  }
};

struct query_response {
  long id;
  bool successful;
  char *data;

  query_response(long id) {
    this->id = id;
    successful = false;
    data = NULL;
  }
};

#endif // QUERY_TYPES_H