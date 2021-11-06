#ifndef PR_CLIENT
#define PR_CLIENT

#include <string>
#include <unistd.h>

struct client_t {
  ~client_t() { close(socket_); }
  client_t();
  void connect(int, const std::string &);
  void send(const std::string &);
  std::string rec();
  int socket_;
};

#endif
