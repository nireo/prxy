#ifndef PR_HEALTH_CHECK_H
#define PR_HEALTH_CHECK_H

#include <memory>
#include <string>
#include <vector>

namespace healthcheck {
class Healthchecker {
public:
  Healthchecker(std::vector<std::string> addrs) : backend_addrs_(addrs) {}
  ~Healthchecker() {}
  void check_server_health(const std::string &);
  std::vector<std::string> backend_addrs_;
};

void startup_server_health_check(std::unique_ptr<Healthchecker>);
}; // namespace healthcheck

#endif
