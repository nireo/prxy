#include "server.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <mutex>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>

void server_pool_t::add(const server_ip_t &addr) {
  std::lock_guard<std::mutex> lock(mut_);

  addrs_.push_back(addr);
}

const server_ip_t &server_pool_t::next() {
  std::lock_guard<std::mutex> lock(mut_);

  size_t idx = cur_index_;
  ++cur_index_;
  if (cur_index_ > addrs_.size())
    cur_index_ = 0;

  return addrs_[idx];
}

void server_t::create_socket(int port) {
  int optval = 1;
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  s_socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  setsockopt(s_socket_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  if (s_socket_ < 0) {
    std::fprintf(stderr, "cannot create server socket\n");
    std::exit(1);
  }

  int bn = bind(s_socket_, (struct sockaddr *)&addr, sizeof(addr));
  if (bn < 0) {
    std::fprintf(stderr, "failed to bind server socket\n");
    std::exit(1);
  }

  int list = listen(s_socket_, 5);
  if (list < 0) {
    std::fprintf(stderr, "failed to listen server socket\n");
    std::exit(1);
  }
}

void server_t::send(const std::string &m, int sock) {
  int snd = ::send(sock, m.c_str(), m.size(), 0);
  if (snd < 0) {
    std::fprintf(stderr, "cannot send data\n");
    std::exit(1);
  }
}

void server_t::accept() {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  c_socket_ = ::accept(s_socket_, (struct sockaddr *)&addr, &len);
  if (c_socket_ < 0) {
    std::fprintf(stderr, "cannot accept connection\n");
    std::exit(1);
  }
}

std::string server_t::rec(int sock) {
  std::string result;
  char buff[4096];
  int size;

  do {
    size = recv(sock, buff, 4096, 0);
    if (size < 0) {
      std::fprintf(stderr, "cannot receive from client.");
      std::exit(1);
    }
    result += buff;
  } while (size - 4096 > 0);

  return result;
}
