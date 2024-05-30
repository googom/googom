//
// Created by root on 5/20/24.
//

#include "tcp_server.h"
#include "tcp_session.h"
#include <seastar/core/seastar.hh>
#include <seastar/net/api.hh>
#include <seastar/net/ip.hh>

#include "../communication_utils/communication_utils.h"

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

/*
 *  HOW a message should look like
 *  COMMAND: key1=value with spaces, key2=another value
 *  MESSAGE: key1=value with spaces, key2=another value | [\"some text\"]
 *  SYSTEM: key1=value with spaces, key2=another value | [\"some text\"]
 *
 * TEST messages
 * COMMAND: user=myuser, topics=topic1 topic2
 * MESSAGE: topic=topic1 | [\"some text\"]
 */
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
                        addr.addr().as_ipv6_address().ip.size());
        session->params["client_address_ipv6"] = str;
    } else {
        session->params["client_address_ipv6"] = "NONE";
    }

    return seastar::do_with(std::move(session), [this](auto& sess) {
        return seastar::repeat([&sess, this] {
            return sess->in.read().then([&sess, this](seastar::temporary_buffer<char> buf) {
                if (!buf) {
                    // No more data to read, stop the iteration
                    return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::yes);
                }

                std::cout << "Request came from ipv4: " << sess->params["client_address_ipv4"] << "\n";
                std::cout << "Request came from ipv6: " << sess->params["client_address_ipv6"] << "\n";
                std::string message(buf.get(), buf.size());
                //IDLE CONNECTION TILL there is a command!!
                //ALL the sessions should be pushed to the internal topics
                
                auto parsedResult = parseInput(message);
                if (parsedResult.type == MessageType::COMMAND) {
                    for (const auto& kv : parsedResult.keyValuePairs) {
                        std::cout << "Key: " << kv.first << ", Value: " << kv.second << std::endl;
                        sess->params[kv.first] = kv.second;
                    }
                    // Continue the loop after processing COMMAND
                    return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::no);
                } else if (parsedResult.type == MessageType::MESSAGE) {
                    // TODO: Check if topic is set
                    // TODO: Check if user has permissions
                    // TODO: Real topic writing

                    for (const auto& kv : parsedResult.keyValuePairs) {
                        std::cout << "Key: " << kv.first << ", Value: " << kv.second << std::endl;
                    }

                    int id = _store.store_message(parsedResult.message);

                    return sess->out.write("Stored message with ID: " + std::to_string(id))
                            .then([&sess] { return sess->out.flush(); })
                            .then([] { return seastar::stop_iteration::no; });
                }

                // If parsedResult type is neither COMMAND nor MESSAGE, continue the loop
                return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::no);
            });
        }).finally([&sess] { return sess->out.close(); });
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
