//
// Created by root on 5/20/24.
//

#include "tcp_server.h"
#include "tcp_session.h"
#include <seastar/core/seastar.hh>
#include <seastar/net/api.hh>
#include <seastar/net/ip.hh>

TcpServer::TcpServer(message_store &store)
        : _store(store) {}

seastar::future<> TcpServer::start(uint16_t port) {
    seastar::listen_options lo;
    lo.reuse_address = true;
    auto listen_addr = seastar::make_ipv4_address({port});
    return seastar::do_with(seastar::engine().listen(listen_addr, lo), [this, &port](auto &server) {
        std::cout << "TCP server listening on port " << port << "\n";
        return seastar::keep_doing([this, &server] {
            return server.accept().then([this](seastar::accept_result ar) {
                return handle_tcp_connection(std::move(ar.connection), ar.remote_address);
            });
        }).finally([&server] {
            server.abort_accept();
        });
    });
}

seastar::future<> TcpServer::handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr) {

    // Create a new session for each connection
    auto session = std::make_unique<TcpSession>(std::move(socket));

    if (addr.addr().is_ipv4()) {
        session->params["client_address_ipv4"] = intToIPv4(addr.addr().as_ipv4_address().ip);
    } else {
        session->params["client_address_ipv6"] = "NONE";
    }

    if (addr.addr().is_ipv6()) {
        std::string str(reinterpret_cast<char *>(addr.addr().as_ipv6_address().ip.begin()),
                        sizeof(addr.addr().as_ipv6_address().ip.size()));
        session->params["client_address_ipv6"] = str;
    } else {
        session->params["client_address_ipv6"] = "NONE";
    }

    return seastar::do_with(std::move(*session), [this](auto &sess) {
        return seastar::repeat([&sess, this] {
            return sess.in.read().then([&sess, this](seastar::temporary_buffer<char> buf) {
                if (buf) {
                    std::cout << "Request came from ipv4: " << sess.params["client_address_ipv4"] << "\n";
                    std::cout << "Request came from ipv6: " << sess.params["client_address_ipv6"] << "\n";
                    std::string message(buf.get(), buf.size());
                    //IDLE CONNECTION TILL there is a command!!
                    //ALL the sessions should be pushed to the internal topics
                    int id = _store.store_message(message);
                    return sess.out.write("Stored message with ID: " + std::to_string(id))
                            .then([&sess] { return sess.out.flush(); })
                            .then([] { return seastar::stop_iteration::no; });
                } else {
                    return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::yes);
                }
            });
        }).finally([&sess] { return sess.out.close(); });
    });
}

std::string TcpServer::intToIPv4(seastar::net::packed<uint32_t> ip) {
    // Break down the integer into its octets
    uint8_t octet1 = (ip >> 24) & 0xFF;
    uint8_t octet2 = (ip >> 16) & 0xFF;
    uint8_t octet3 = (ip >> 8) & 0xFF;
    uint8_t octet4 = ip & 0xFF;

    // Concatenate the octets into a string
    return std::to_string(octet1) + "." +
           std::to_string(octet2) + "." +
           std::to_string(octet3) + "." +
           std::to_string(octet4);
}
