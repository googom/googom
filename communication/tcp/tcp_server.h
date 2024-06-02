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
    message_store& _store;
    std::unordered_map<std::string, std::unordered_set<TcpSession*>> _subscriptions;
    void cleanup_session(TcpSession* session);


public:
    explicit TcpServer(message_store& store);
    seastar::future<> start(uint16_t port);
    seastar::future<> handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr);

    std::string intToIPv4(seastar::net::packed<uint32_t> ip);

    // Method to notify clients about new messages
    void notify_subscribers(const std::string& topic, const std::string& message);
    void add_subscription(const std::string& topic, TcpSession* session);
    void remove_subscription(const std::string& topic, TcpSession* session);
};

#endif // GOOGOM_TCP_SERVER_H
