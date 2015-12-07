#ifndef TABLE_CONNECTION_H
#define TABLE_CONNECTION_H

/**
 * Contains the ZMQ socket to send messages over and the appropriate values to 
 * lock on it. Note that it is necessary to lock the socket because it would 
 * be very bad if multiple things tried to communicate with it at the same 
 * time! Would either produce undefined behaviour or cause dropped messages.
 */
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