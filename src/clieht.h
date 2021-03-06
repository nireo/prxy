#ifndef PR_CLIENT
#define PR_CLIENT

#include <string>

struct client_t {
  void connect(int, const std::string &);
  void send(const std::string &);
  std::string rec();
  int socket;
};

#endif
