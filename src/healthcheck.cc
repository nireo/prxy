#include "healthcheck.h"
#include <chrono>
#include <cstddef>
#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>

void healthcheck::Healthchecker::check_server_health(const std::string &addr) {
  auto client = drogon::HttpClient::newHttpClient(addr);
  auto req = drogon::HttpRequest::newHttpRequest();

  req->setMethod(drogon::Get);
  req->setPath("/healthcheck");

  // send a request and give it one second for timeout.
  bool ok = false;
  client->sendRequest(
      req,
      [&](drogon::ReqResult, const drogon::HttpResponsePtr &response) {
        std::cout << "[HEALTHCHECK] server: " << addr << " is [up]\n";
        ok = true;
      },
      1);

  if (!ok) {
    throw std::runtime_error("cannot connect to server: " + addr +
                             " in allocated time.");
  }
}

// this function should be run in a thread.
void healthcheck::startup_server_health_check(
    std::unique_ptr<Healthchecker> hc) {

  for (;;) {
    for (const auto &addr : hc->backend_addrs_) {
      try {
        hc->check_server_health(addr);
      } catch (std::exception &e) {
        // add some better actions when servers fail
        std::cout << e.what() << '\n';
      }
    }

    std::this_thread::sleep_for(std::chrono::minutes() * 1);
  }
}
