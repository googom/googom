//
// Created by root on 4/22/24.
//

#include "simple_http_server.h"

SimpleHttpServer::SimpleHttpServer(boost::asio::io_context& ioc, boost::asio::ip::tcp::tcp::endpoint endpoint)
: ioc_(ioc), acceptor_(ioc, endpoint) {
    acceptConnections();
}

void SimpleHttpServer::acceptConnections() {
    acceptor_.async_accept(
            [this](boost::system::error_code ec, boost::asio::ip::tcp::tcp::socket socket) {
                if (!ec) {
                    std::make_shared<HttpSession>(std::move(socket))->start();
                }
                acceptConnections();
            });
}