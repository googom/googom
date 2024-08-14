//
// Created by root on 5/20/24.
//

#ifndef GOOGOM_TCP_SERVER_H
#define GOOGOM_TCP_SERVER_H

#include "../temp_store/message_store.h"
#include <seastar/core/future.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/net/inet_address.hh>
#include "tcp_session.h"

class TcpServer {
public:
    explicit TcpServer();
    seastar::future<> start(uint16_t port);
    seastar::future<> handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr);

    std::string intToIPv4(seastar::net::packed<uint32_t> ip);

    void addSubscription(const std::string& topic, TcpSession* session);
    void removeSubscription(const std::string& topic, TcpSession* session);

private:
    void cleanup_session(TcpSession* session);
    std::vector<uint8_t> stringToVector(const std::string& str);
};



#endif // GOOGOM_TCP_SERVER_H




