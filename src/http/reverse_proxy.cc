#include "reverse_proxy.h"

using namespace drogon;
using namespace reverse_proxy;

void ReverseProxy::initAndStart(const Json::Value &conf) {
  if (conf.isMember("backends") && conf["backends"].isArray()) {
    for (const auto &backend : conf["backends"]) {
      addrs_.emplace_back(backend.asString());
    }

    if (addrs_.empty()) {
      std::fprintf(stderr, "you need at least one backend server.\n");
      std::exit(1);
    }
  } else {
    std::fprintf(stderr, "backends not defined in configuration.\n");
    std::exit(1);
  }

  pipeline_depth_ = conf.get("pipelining", 0).asInt();
  same_client_backend_ =
      conf.get("same_client_to_same_backend", false).asBool();
  conn_factor_ = conf.get("connection_factor", 1).asInt();
  if (conn_factor_ == 0 || conn_factor_ > 100) {
    std::fprintf(stderr, "%zu is an invalid connection factor", conn_factor_);
    std::exit(1);
  }

  clients_.init(
      [this](std::vector<HttpClientPtr> &clients, size_t io_loop_index) {
        clients.resize(addrs_.size() * conn_factor_);
      });

  client_index_.init(
      [this](size_t &index, size_t ioLoopIndex) { index = ioLoopIndex; });
  drogon::app().registerPreRoutingAdvice([this](const HttpRequestPtr &req,
                                                AdviceCallback &&callback,
                                                AdviceChainCallback &&pass) {
    pre_routing(req, std::move(callback), std::move(pass));
  });
}
