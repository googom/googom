#ifndef GOOGOM_REST_SERVER_H
#define GOOGOM_REST_SERVER_H

#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/future.hh>
#include <seastar/http/httpd.hh>
#include <seastar/http/function_handlers.hh>
#include <seastar/net/api.hh>
#include <seastar/core/sleep.hh>
#include <iostream>
#include <unordered_map>
#include <seastar/json/json_elements.hh>

#include "message_store.h"

// TODO security is missing!
class RestServer {
    seastar::httpd::http_server_control _server;
    message_store& _store;

    std::string decode64(const std::string &val);
    std::string encode64(const std::string &val);

public:
    RestServer(message_store& store);
    void start(uint16_t port);
    seastar::future<> stop();
    void setup_routes();
};

#endif // GOOGOM_REST_SERVER_H
