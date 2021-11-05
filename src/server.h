#ifndef PR_SERVER_H
#define PR_SERVER_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

constexpr std::uint16_t SERVER_MAX_COUNT = 64;

struct server_t {
  explicit server_t(const std::string &addr) : addr_(addr){};
  int accept();
  const std::string &rec(int);

  std::string addr_;
  std::uint16_t port_;
};

struct server_ip_t {
  explicit server_ip_t(std::string addr, std::uint16_t port)
      : addr_(addr), port_(port) {}
  std::string addr_;
  std::uint16_t port_;

  bool operator==(const server_ip_t &other) const noexcept {
    return addr_ == other.addr_ && other.port_ == port_;
  }
};

struct server_pool_t {
  std::vector<server_ip_t> addrs_;
  std::size_t len_;
  std::size_t cur_index_;
  std::mutex mut_;

  explicit server_pool_t() : addrs_(), len_(0), cur_index_(0) {}
};

#endif
