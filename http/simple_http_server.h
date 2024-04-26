//
// Created by root on 4/22/24.
//

#ifndef GOOGOM_SIMPLE_HTTP_SERVER_H
#define GOOGOM_SIMPLE_HTTP_SERVER_H


#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "http_session.h"

class SimpleHttpServer {
    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    bool accepting_connections_; // Flag to control accepting connections

public:
    SimpleHttpServer(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint)
            : ioc_(ioc), acceptor_(ioc, endpoint), accepting_connections_(true) {
        acceptConnections(); // Start accepting connections
    }

    // Stop accepting new connections
    void stop() {
        accepting_connections_ = false;
    }

private:
    void acceptConnections() {
        acceptor_.async_accept(
                [this](boost::system::error_code ec, boost::asio::ip::tcp::tcp::socket socket) {
                    if (!ec && accepting_connections_) {
                        std::make_shared<HttpSession>(std::move(socket))->start();
                    }
                    if (accepting_connections_) {
                        acceptConnections(); // Continue accepting connections
                    }
                });
    }
};

#endif //GOOGOM_SIMPLE_HTTP_SERVER_H
