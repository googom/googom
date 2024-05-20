#include "topics/topic_debugging/topic_debugging.h"
#include "topics/topic_public/topic_public_definition.h"
#include "topics/topic_public/topic_public_message.h"
#include "topics/topic_private/topic_offset/topic_private_offset_definition.h"

#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>

#include "communication/http/rest_server.h"
#include "communication/tcp/tcp_server.h"
#include "communication/temp_store/message_store.h"
#include "managers/config_manager.h"
#include "managers/initialization_manager.h"

#include <iostream>


int main(int argc, char **argv) {
    //!!The first stage

    seastar::app_template app;

    app.add_options()
            ("config,c", boost::program_options::value<std::string>(), "this is for tcp port");

    message_store store;
    RestServer rest(store);
    TcpServer tcpServer(store);


    return app.run(argc, argv, [&app, &rest, &tcpServer]() -> seastar::future<> {
        auto &args = app.configuration();
        auto configFile = args["config"].as<std::string>();

        InitializationManager initializationManager = InitializationManager();
        initializationManager.initialize(configFile);

        uint16_t restPort = ConfigManager::getInstance()->getWebPort();
        uint16_t tcpPort = ConfigManager::getInstance()->getTcpPort();

        message_store store;

        // Start REST server
        rest.start(restPort);

        // Start TCP server
        return tcpServer.start(tcpPort);
    });


}
