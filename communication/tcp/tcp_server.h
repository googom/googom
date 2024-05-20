//
// Created by root on 5/20/24.
//

#ifndef GOOGOM_TCP_SERVER_H
#define GOOGOM_TCP_SERVER_H

#include "../temp_store/message_store.h"
#include <seastar/core/future.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/net/inet_address.hh>

class TcpServer {
    message_store& _store;

public:
    TcpServer(message_store& store);
    seastar::future<> start(uint16_t port);
    seastar::future<> handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr);
};

#endif // GOOGOM_TCP_SERVER_H
