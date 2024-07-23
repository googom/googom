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
#include "../../managers/notification_manager.h"

class TcpServer {
public:
    explicit TcpServer();
    seastar::future<> start(uint16_t port);
    seastar::future<> handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr);

    std::string intToIPv4(seastar::net::packed<uint32_t> ip);

    seastar::future<> add_subscription(const std::string& topic, TcpSession* session);
    seastar::future<> remove_subscription(const std::string& topic, TcpSession* session);

    seastar::future<> debug_handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr);

private:
    std::unordered_map<std::string, std::unordered_set<TcpSession*>> _subscriptions;
    void cleanup_session(TcpSession* session);
    NotificationManager _notification_manager;
};



#endif // GOOGOM_TCP_SERVER_H




