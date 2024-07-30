//
// Created by root on 5/20/24.
//

#ifndef GOOGOM_TCP_SESSION_H
#define GOOGOM_TCP_SESSION_H

#include <seastar/core/future.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/net/inet_address.hh>
#include <seastar/core/seastar.hh>
#include <seastar/net/api.hh>
#include <iostream>
#include <map>
#include <queue>

class TcpSession : public seastar::enable_shared_from_this<TcpSession> {
public:
    std::map<std::string, std::string> params;  // Session parameters
    std::queue<std::string> message_queue;      // Queue to hold messages to be sent

    // Function to enqueue messages
    void enqueue_message(const std::string &message) {
        message_queue.push(message);
    }

    // Function to check if there are messages pending
    bool has_pending_messages() const {
        return !message_queue.empty();
    }
};


#endif //GOOGOM_TCP_SESSION_H
