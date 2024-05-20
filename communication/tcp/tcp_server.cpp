//
// Created by root on 5/20/24.
//

#include "tcp_server.h"
#include <seastar/core/seastar.hh>
#include <seastar/net/api.hh>

TcpServer::TcpServer(message_store &store)
        : _store(store) {}

seastar::future<> TcpServer::start(uint16_t port) {
    seastar::listen_options lo;
    lo.reuse_address = true;
    auto listen_addr = seastar::make_ipv4_address({port});
    return seastar::do_with(seastar::engine().listen(listen_addr, lo), [this, &port](auto& server) {
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
    auto in = socket.input();
    auto out = socket.output();
    return seastar::do_with(std::move(socket), std::move(in), std::move(out),
                            [this](auto& socket, auto& in, auto& out) {
                                return seastar::repeat([&in, &out, this] {
                                    return in.read().then([&out, this](seastar::temporary_buffer<char> buf) {
                                        if (buf) {
                                            std::string message(buf.get(), buf.size());
                                            int id = _store.store_message(message);
                                            return out.write("Stored message with ID: " + std::to_string(id))
                                                    .then([&out] { return out.flush(); })
                                                    .then([] { return seastar::stop_iteration::no; });
                                        } else {
                                            return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::yes);
                                        }
                                    });
                                }).finally([&out] { return out.close(); });
                            });
}
