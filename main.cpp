#include "topics/topic_debugging/topic_debugging.h"
#include "topics/topic_public/topic_public_definition.h"
#include "topics/topic_public/topic_public_message.h"
#include "topics/topic_private/topic_offset/topic_private_offset_definition.h"

#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>

#include "communication/http/rest_server.h"
#include "communication/http/message_store.h"
#include "managers/config_manager.h"
#include "managers/initialization_manager.h"

#include <iostream>

// TCP Server Handler
seastar::future<> handle_tcp_connection(seastar::connected_socket socket, seastar::socket_address addr) {
    auto in = socket.input();
    auto out = socket.output();

    return seastar::do_with(std::move(socket), std::move(in), std::move(out),
                            [](auto &socket, auto &in, auto &out) {
                                return seastar::repeat([&in, &out] {
                                    return in.read().then([&out](seastar::temporary_buffer<char> buf) {
                                        if (buf) {
                                            std::string message = "hello " + std::string(buf.get(), buf.size());
                                            return out.write(message).then([&out] {
                                                return out.flush();
                                            }).then([] {
                                                return seastar::stop_iteration::no;
                                            });
                                        } else {
                                            return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::yes);
                                        }
                                    }).handle_exception([](std::exception_ptr eptr) {
                                        try {
                                            std::rethrow_exception(eptr);
                                        } catch (const std::exception &e) {
                                            std::cerr << "Error reading from input stream: " << e.what() << '\n';
                                        }
                                        return seastar::make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::yes);
                                    });
                                }).finally([&out] {
                                    return out.close().handle_exception([](std::exception_ptr eptr) {
                                        try {
                                            std::rethrow_exception(eptr);
                                        } catch (const std::exception &e) {
                                            std::cerr << "Error closing output stream: " << e.what() << '\n';
                                        }
                                    });
                                });
                            });
}

int main(int argc, char **argv) {
    //!!The first stage

    seastar::app_template app;

    app.add_options()
            ("config,c", boost::program_options::value<std::string>(), "this is for tcp port");

    message_store store;
    RestServer rest(store);


    return app.run(argc, argv, [&app, &rest, &store] {

        auto &args = app.configuration();
        auto configFile = args["config"].as<std::string>();


        InitializationManager initializationManager = InitializationManager();
        initializationManager.initialize(configFile);

        uint16_t restPort = ConfigManager::getInstance()->getWebPort();
        uint16_t tcpPort = ConfigManager::getInstance()->getTcpPort();

        // Start REST server
        rest.start(restPort);

        // Start TCP server
        seastar::listen_options lo;
        lo.reuse_address = true;
        auto listen_addr = seastar::make_ipv4_address({tcpPort});

        auto &engine = seastar::engine();
        return do_with(engine.listen(listen_addr, lo), [&](auto &server) {
            std::cout << "TCP server listening on port " << tcpPort << "\n";
            return seastar::keep_doing([&server] {
                return server.accept().then([](seastar::accept_result ar) {
                    // Handle TCP connection
                    return handle_tcp_connection(std::move(ar.connection), ar.remote_address)
                            .then_wrapped([](seastar::future<> f) {
                                try {
                                    f.get();  // This will throw if the future failed
                                } catch (const std::exception &e) {
                                    std::cerr << "Failed to handle TCP connection: " << e.what() << '\n';
                                }
                            });
                });
            }).finally([&server, &tcpPort] {
                server.abort_accept();
                std::cout << "Closing TCP server on port " << tcpPort << "\n";
            });
        });


    });


}
