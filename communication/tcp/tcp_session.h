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

class TcpSession {
public:
    seastar::connected_socket socket;
    seastar::input_stream<char> in;
    seastar::output_stream<char> out;
    std::map<std::string, std::string> params;  // Session parameters

    TcpSession(seastar::connected_socket cs)
            : socket(std::move(cs)), in(socket.input()), out(socket.output()) {}
};


#endif //GOOGOM_TCP_SESSION_H
