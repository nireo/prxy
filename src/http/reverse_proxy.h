#include <drogon/HttpClient.h>
#include <drogon/IOThreadStorage.h>
#include <drogon/drogon.h>
#include <drogon/drogon_callbacks.h>
#include <drogon/plugins/Plugin.h>
#include <drogon/utils/FunctionTraits.h>

namespace reverse_proxy {
class ReverseProxy : public drogon::Plugin<ReverseProxy> {
  ReverseProxy() {}
  virtual void initAndStart(const Json::Value &) override;
  virtual void shutdown() override;

private:
  drogon::IOThreadStorage<std::vector<drogon::HttpClientPtr>> clients_;
  drogon::IOThreadStorage<size_t> client_index_{0};
  std::vector<std::string> addrs_;
  bool same_client_backend_{false};
  size_t pipeline_depth_{0};
  size_t conn_factor_{1};

  void pre_routing(const drogon::HttpRequestPtr &, drogon::AdviceCallback &&,
                   drogon::AdviceChainCallback &&);
};
} // namespace reverse_proxy
