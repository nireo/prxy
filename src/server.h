#ifndef PR_SERVER_H
#define PR_SERVER_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

constexpr std::uint16_t SERVER_MAX_COUNT = 64;

struct server_t {
  explicit server_t() : s_socket_(-1), c_socket_(-1){};
  ~server_t() { close(s_socket_); }
  void create_socket(int);
  void send(const std::string &, int);
  void accept();
  std::string rec(int);

  int s_socket_;
  int c_socket_;
};

struct server_ip_t {
  explicit server_ip_t(std::string addr, std::uint16_t port)
      : addr_(addr), port_(port) {}
  std::string addr_;
  uint16_t port_;

  bool operator==(const server_ip_t &other) const noexcept {
    return addr_ == other.addr_ && other.port_ == port_;
  }
};

struct server_pool_t {
  std::vector<server_ip_t> addrs_;
  size_t cur_index_;
  std::mutex mut_;

  [[nodiscard]] const server_ip_t &next();
  void add(const server_ip_t &);
  explicit server_pool_t() : addrs_(), cur_index_(0), mut_() {}
};

#endif
