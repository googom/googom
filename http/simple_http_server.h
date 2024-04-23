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
    boost::asio::ip::tcp::tcp::acceptor acceptor_;

public:
    SimpleHttpServer(boost::asio::io_context& ioc, boost::asio::ip::tcp::tcp::endpoint endpoint);

private:
    void acceptConnections();
};

#endif //GOOGOM_SIMPLE_HTTP_SERVER_H
