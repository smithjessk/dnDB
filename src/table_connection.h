#ifndef TABLE_CONNECTION_H
#define TABLE_CONNECTION_H

struct table_connection {
  std::string address;
  bool waiting_for_reply;
  bool table_deleted;
  std::mutex mutex;
  std::condition_variable cv;
  zmq::context_t context;
  zmq::socket_t socket;

  table_connection(int port)
    : context(1),
    socket(context, ZMQ_REQ) {
      std::stringstream ss;
      ss << "tcp://localhost:";
      ss << port;
      ss >> address;
      socket.connect(address);
      waiting_for_reply = false;
      table_deleted = false;
  }

  void mark_deleted() {
    table_deleted = true;
  }
};

#endif // TABLE_CONNECTION_H