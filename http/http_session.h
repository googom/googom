#ifndef GOOGOM_HTTP_SESSION_H
#define GOOGOM_HTTP_SESSION_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

class HttpSession : public std::enable_shared_from_this<HttpSession> {
    boost::asio::ip::tcp::tcp::socket socket_;
    boost::beast::flat_buffer buffer_; // Efficiently sized buffer for operations
    boost::beast::http::request<boost::beast::http::dynamic_body> request_;

public:
    explicit HttpSession(boost::asio::ip::tcp::tcp::socket socket);

    void start();

private:
    void readRequest();

    void writeResponse();
};

#endif // GOOGOM_HTTP_SESSION_H