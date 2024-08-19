//
// Created by root on 5/20/24.
//

#include "tcp_server.h"
#include <seastar/core/seastar.hh>
#include <seastar/net/api.hh>
#include <seastar/net/ip.hh>

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

#include "../communication_utils/communication_utils.h"
#include "../../topics/topic_public/topic_public_definition.h"
#include "../../topics/topic_public/topic_public_message.h"
#include "../../managers/distributed_topic_manager.h"
#include "../../managers/subscription_manager.h"

// Include DistributedTopicManager
extern seastar::distributed<DistributedTopicManager> distributedTopicManager;

auto &manager = SubscriptionManager::getInstance();

TcpServer::TcpServer() = default;

// TcpServer::start function
seastar::future<> TcpServer::start(uint16_t port) {
    seastar::listen_options lo;
    lo.reuse_address = true;
    auto listen_addr = seastar::make_ipv4_address({port});
    auto server = seastar::engine().listen(listen_addr, lo);
    std::cout << "TCP server listening on port " << port << "\n";

    // Use a while (true) loop to continuously accept connections
    return seastar::do_with(std::move(server), [this](auto &server) {
        return seastar::do_until(
                [this] { return false; },
                [this, &server] {
                    return server.accept().then([this](seastar::accept_result ar) {
                        // Handle the accepted connection asynchronously
                        (void) handle_tcp_connection(std::move(ar.connection),
                                                     ar.remote_address).handle_exception(
                                [](std::exception_ptr eptr) {
                                    try {
                                        std::rethrow_exception(eptr);
                                    } catch (const std::exception &e) {
                                        std::cerr << "Failed to handle connection: " << e.what() << "\n";
                                    }
                                });
                        // Continue the loop
                        return seastar::make_ready_future<>();
                    }).handle_exception([](std::exception_ptr eptr) {
                        try {
                            std::rethrow_exception(eptr);
                        } catch (const std::exception &e) {
                            std::cerr << "Failed to accept connection: " << e.what() << "\n";
                        }
                        // Continue the loop even if accept fails
                        return seastar::make_ready_future<>();
                    });
                }
        ).finally([&server] {
            server.abort_accept();
        });
    });
}


/*
 *  HOW a message should look like
 *  COMMAND: key1=value with spaces, key2=another value, type=writer
 *  MESSAGE: key1=value with spaces, key2=another value | [\"some text\"]
 *  SYSTEM: key1=value with spaces, key2=another value | [\"some text\"]
 *  // types in command: WRITER, READER, BROKER
 *
 * TEST messages
 * COMMAND: user=myuser, topics=topic1 topic2 message, type=reader
 * MESSAGE: topic=topic1 | [\"some text\"]
 * MESSAGE: topic=message | [\"some text\"]
 */


seastar::future<> TcpServer::handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr) {

    auto in = socket.input();
    auto out = socket.output();

    // Create a new session for each connection
    auto session = seastar::make_shared<TcpSession>();

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

    return seastar::do_with(std::move(in), std::move(out), std::move(session),
                            [this](auto &in, auto &out, auto &sess) {
                                return seastar::repeat([&in, &out, &sess, this] {

                                    return in.read().then([&sess, &out, this](seastar::temporary_buffer<char> buf) {
                                        if (!buf) {
                                            // No more data to read, stop the iteration
                                            return seastar::make_ready_future<seastar::stop_iteration>(
                                                    seastar::stop_iteration::yes);
                                        }

                                        std::cout << "Request came from ipv4: " << sess->params["client_address_ipv4"]
                                                  << "\n";
                                        std::cout << "Request came from ipv6: " << sess->params["client_address_ipv6"]
                                                  << "\n";
                                        std::string message(buf.get(), buf.size());
                                        //IDLE CONNECTION TILL there is a command!!
                                        //ALL the sessions should be pushed to the internal topics

                                        std::vector<std::string> topicsToSubscribe;
                                        std::string type;

                                        auto parsedResult = parseInput(message);
                                        if (parsedResult.type == MessageType::COMMAND) {

                                            //TODO read command parameters such as starting offset or timestamp
                                            for (const auto &kv: parsedResult.keyValuePairs) {
                                                std::cout << "Key: " << kv.first << ", Value: " << kv.second
                                                          << std::endl;
                                                sess->params[kv.first] = kv.second;

                                                if (kv.first == "type") {
                                                    std::istringstream ss(kv.second);
                                                    ss >> type;
                                                }

                                                // Handle topic subscription
                                                if (kv.first == "topics") {
                                                    std::istringstream ss(kv.second);
                                                    std::string topic;

                                                    while (ss >> topic) {
                                                        topicsToSubscribe.push_back(topic);
                                                    }
                                                }
                                            }

                                            //TODO broker should be added
                                            if (type == "reader") {
                                                //TODO AUTH check

                                                sess->sessionType = SessionType::READER;
                                                if (!topicsToSubscribe.empty()) {
                                                    for (const auto &topic: topicsToSubscribe) {
                                                        addSubscription(topic, sess.get());
                                                    }
                                                }






                                                return seastar::repeat([this, &out, &sess, topicsToSubscribe, i = size_t(0)]() mutable {
                                                    if (i >= topicsToSubscribe.size()) {
                                                        // If all topics are processed, reset the index and continue the loop
                                                        i = 0;
                                                        return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::no);
                                                    }

                                                    const std::string &topic = topicsToSubscribe[i];
                                                    i++;

                                                    return distributedTopicManager.local().getUnconsumedMessages(
                                                            topic, sess->sessionId).then([this, &out, &sess, topic](std::vector<TopicPublicMessage> messages) {
                                                        if (!messages.empty()) {
                                                            return process_messages(out, messages);
                                                        }
                                                        return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::no);
                                                    }).handle_exception([](std::exception_ptr eptr) {
                                                        try {
                                                            std::rethrow_exception(eptr);
                                                        } catch (const std::exception &e) {
                                                            std::cerr << "Error processing messages: " << e.what() << "\n";
                                                        }
                                                        return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::no);
                                                    });
                                                });




                                            } else if (type == "writer") {
                                                sess->sessionType = SessionType::WRITER;
                                            } else if (type == "broker_reader") {
                                                sess->sessionType = SessionType::BROKER_READER;
                                            } else if (type == "broker_writer") {
                                                sess->sessionType = SessionType::BROKER_WRITER;
                                            }

                                            // Continue the loop after processing COMMAND
                                            return seastar::make_ready_future<seastar::stop_iteration>(
                                                    seastar::stop_iteration::no);
                                        } else if (parsedResult.type == MessageType::MESSAGE) {
                                            // TODO: Check if user has permissions
                                            // TODO: Real topic writing

                                            // TODO auth check

                                            if (sess->sessionType != SessionType::WRITER &&
                                                sess->sessionType != SessionType::BROKER_WRITER) {
                                                //TODO throw an error message to the client

                                                return out.write(
                                                                "ERROR\n")
                                                        .then([&out, &sess] { return out.flush(); })
                                                        .then([] { return seastar::stop_iteration::no; });
                                            }

                                            for (const auto &kv: parsedResult.keyValuePairs) {
                                                std::cout << "Key: " << kv.first << ", Value: " << kv.second
                                                          << std::endl;
                                            }

                                            // Accessing the DistributedTopicManager to store message
                                            std::string topic = parsedResult.keyValuePairs["topic"];
                                            if (topic.empty()) {
                                                //TODO throw an error message to the client

                                                return out.write(
                                                                "ERROR\n")
                                                        .then([&out, &sess] { return out.flush(); })
                                                        .then([] { return seastar::stop_iteration::no; });
                                            }

                                            std::string filename = topic + ".arrow";
                                            return distributedTopicManager.local().getOrCreateTopicPublicDefinition(
                                                            topic, 0, 1024, filename)
                                                    .then([this, &out, &sess, parsedResult](
                                                            const std::shared_ptr<TopicPublicDefinition> &topicDef) {
                                                        // Insert the message into the topic
                                                        TopicPublicMessage
                                                                topicMessage(
                                                                topicDef->getTopicName(),
                                                                "keys_placeholder",
                                                                "headers_placeholder",
                                                                stringToVector(parsedResult.message)
                                                        );
                                                        auto offset = topicDef->insert(topicMessage);

                                                        distributedTopicManager.local().enqueueMessage(topicMessage);

                                                        return out.write(
                                                                        "Stored message with ID: " + offset.str() +
                                                                        "\n")
                                                                .then([&out, &sess] { return out.flush(); })
                                                                .then([] { return seastar::stop_iteration::no; });
                                                    });
                                        }

                                        // If parsedResult type is neither COMMAND nor MESSAGE, continue the loop
                                        return seastar::make_ready_future<seastar::stop_iteration>(
                                                seastar::stop_iteration::no);
                                    });

                                }).finally([&out, &sess, this] {
                                    // Cleanup the session on disconnection
                                    cleanup_session(sess.get());
                                    return out.close();
                                });
                            });
}

void TcpServer::cleanup_session(TcpSession *session) {
    for (const auto &kv: session->params) {
        if (kv.first == "topics") {
            std::istringstream ss(kv.second);
            std::string topic;
            while (ss >> topic) {
                removeSubscription(topic, session);
            }
        }
    }
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

void TcpServer::addSubscription(const std::string &topic, TcpSession *session) {
    manager.addSubscription(topic, session->sessionId);
}

void TcpServer::removeSubscription(const std::string &topic, TcpSession *session) {
    manager.removeSubscription(session->sessionId);
}

// Function to convert std::string to std::vector<uint8_t>
std::vector<uint8_t> TcpServer::stringToVector(const std::string &str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}


seastar::future<seastar::stop_iteration>
TcpServer::process_messages(seastar::output_stream<char> &out, const std::vector<TopicPublicMessage> &messages) {
    for (const auto &message: messages) {
        out.write("Message content: " + std::string(message.getValue().begin(), message.getValue().end()) + "\n").get();
        out.flush().get();
    }

    return seastar::make_ready_future<seastar::stop_iteration>(
            seastar::stop_iteration::no);
}




