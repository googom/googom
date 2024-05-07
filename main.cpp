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
    return do_with(std::move(socket), [](auto &socket) {
        auto in = socket.input();
        auto out = socket.output();
        return seastar::repeat([&in, &out] {
            return in.read().then([&out](auto buf) {
                if (buf) {
                    return out.write(std::move(buf)).then([&out] {
                        return out.flush();
                    }).then([] {
                        return seastar::stop_iteration::no;
                    });
                } else {
                    return make_ready_future<seastar::stop_iteration>(seastar::stop_iteration::yes);
                }
            });
        }).finally([&out] {
            return out.close();
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
        return do_with(seastar::engine().listen(seastar::make_ipv4_address({tcpPort}), lo), [](auto &server) {
            return seastar::keep_doing([&server] {
                return server.accept().then([](seastar::accept_result ar) {
                    // Instead of detaching, manage the future
                    handle_tcp_connection(std::move(ar.connection), ar.remote_address)
                            .then_wrapped([](seastar::future<> f) {
                                try {
                                    f.get();  // This will throw if the future failed
                                } catch (const std::exception &e) {
                                    std::cerr << "Failed to handle TCP connection: " << e.what() << '\n';
                                }
                            });
                });
            });
        }).then([restPort, tcpPort] {
            std::cout << "REST server listening on port " << restPort << "\n";
            std::cout << "TCP server listening on port " << tcpPort << "\n";
        });
    });


}
