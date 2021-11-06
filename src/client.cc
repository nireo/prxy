#include "client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

client_t::client_t() {
  socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_ < 0) {
    std::fprintf(stderr, "cannot create client socket.");
    std::exit(1);
  }
}

void client_t::connect(int port, const std::string &addr) {
  struct sockaddr_in addr_;
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  addr_.sin_addr.s_addr = inet_addr(addr.c_str());

  int conn;
  conn = ::connect(socket_, (struct sockaddr *)&addr_, sizeof(addr_));
  if (conn < 0) {
    std::fprintf(stderr, "cannot connect to server.\n");
    std::exit(1);
  }
}

std::string client_t::rec() {
  std::string data;
  char buff[4096];
  int size;

  do {
    size = recv(socket_, buff, 4096, 0);
    if (size < 0) {
      std::fprintf(stderr, "cannot receive data from the client.");
      std::exit(1);
    }
    data += buff;
  } while (size - 4096 > 0);

  return data;
}
