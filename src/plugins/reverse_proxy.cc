#include "reverse_proxy.h"
#include "../healthcheck.h"
#include <drogon/drogon_callbacks.h>
#include <functional>
#include <trantor/net/EventLoop.h>

using namespace drogon;
using namespace reverse_proxy;

void ReverseProxy::initAndStart(const Json::Value &conf) {
  std::vector<std::string> healthcheck_addrs;
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
  auto hc = std::make_unique<healthcheck::Healthchecker>(healthcheck_addrs);
  healthcheck::startup_server_health_check(std::move(hc));

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

void ReverseProxy::pre_routing(const HttpRequestPtr &req,
                               AdviceCallback &&callback,
                               AdviceChainCallback &&) {
  size_t index;
  auto &clients_vector = *clients_;

  if (same_client_backend_) {
    index = std::hash<uint32_t>{}(req->getPeerAddr().ipNetEndian()) %
            clients_vector.size();
    index =
        (index + (++(*client_index_)) * addrs_.size()) % clients_vector.size();
  } else {
    index = ++(*client_index_) % clients_vector.size();
  }

  auto &client_ptr = clients_vector[index];
  if (!client_ptr) {
    auto &addr = addrs_[index % addrs_.size()];
    client_ptr = HttpClient::newHttpClient(
        addr, trantor::EventLoop::getEventLoopOfCurrentThread());
    client_ptr->setPipeliningDepth(pipeline_depth_);
  }

  req->setPassThrough(true);
  client_ptr->sendRequest(
      req, [callback = std::move(callback)](ReqResult result,
                                            const HttpResponsePtr &resp) {
        if (result == ReqResult::Ok) {
          resp->setPassThrough(true);
          callback(resp);
        } else {
          auto errResp = HttpResponse::newHttpResponse();
          errResp->setStatusCode(k500InternalServerError);
          callback(errResp);
        }
      });
}
