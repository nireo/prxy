#ifndef PR_PROXY_H
#define PR_PROXY_H

#include "server.h"
#include <memory>

struct proxy_t {
  std::unique_ptr<server_pool_t> pool_ = nullptr;
  uint16_t host_port_;
  int current_socket_;
};

#endif
