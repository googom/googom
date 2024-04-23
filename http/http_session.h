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
    void readRequest() {
        auto self = shared_from_this();
        boost::beast::http::async_read(socket_, buffer_, request_,
                         [this, self](boost::beast::error_code ec, std::size_t) {
                             if (!ec) writeResponse();
                         });
    }

    void writeResponse() {
        auto self = shared_from_this();
        boost::beast::http::response<boost::beast::http::string_body> response{
                boost::beast::http::status::ok, request_.version()};
        response.set(boost::beast::http::field::server, "Boost Beast Server");
        response.set(boost::beast::http::field::content_type, "text/plain");
        response.body() = "Hello, world!";
        response.prepare_payload();

        boost::beast::http::async_write(socket_, response,
                          [this, self](boost::beast::error_code ec, std::size_t) {
                              socket_.shutdown(boost::asio::ip::tcp::tcp::socket::shutdown_send, ec);
                              socket_.close(ec);
                          });
    }
};

#endif // GOOGOM_HTTP_SESSION_H