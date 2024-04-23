#include "http_session.h"

HttpSession::HttpSession(boost::asio::ip::tcp::tcp::socket socket) : socket_(std::move(socket)) {
socket_.set_option(boost::asio::ip::tcp::tcp::no_delay(true));  // Disable Nagle's algorithm
}

void HttpSession::start() {
    HttpSession::readRequest();
}