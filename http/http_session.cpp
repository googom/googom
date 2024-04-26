#include "http_session.h"

HttpSession::HttpSession(boost::asio::ip::tcp::tcp::socket socket) : socket_(std::move(socket)) {
socket_.set_option(boost::asio::ip::tcp::tcp::no_delay(true));  // Disable Nagle's algorithm
}

void HttpSession::start() {
    HttpSession::readRequest();
}

void HttpSession::readRequest() {
    auto self = shared_from_this();
    boost::beast::http::async_read(socket_, buffer_, request_,
                                   [this, self](boost::beast::error_code ec, std::size_t) {
                                       if (!ec) writeResponse();
                                   });
}

void HttpSession::writeResponse() {
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