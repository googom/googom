//
// Created by root on 5/20/24.
//

#ifndef GOOGOM_TCP_SESSION_H
#define GOOGOM_TCP_SESSION_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <seastar/core/future.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/net/inet_address.hh>
#include <seastar/core/seastar.hh>
#include <seastar/net/api.hh>
#include <iostream>
#include <map>
#include <queue>

enum class SessionType {
    READER,
    WRITER,
    BROKER_READER,
    BROKER_WRITER
};

class TcpSession : public seastar::enable_shared_from_this<TcpSession> {
public:
    std::map<std::string, std::string> params;  // Session parameters
    std::string sessionId;
    SessionType sessionType;

    // Constructor
    TcpSession() {
        // Generate a time-based UUID and convert it to a string
        boost::uuids::uuid uuid = boost::uuids::basic_random_generator<boost::mt19937>()();
        sessionId = boost::uuids::to_string(uuid);
    }
};


#endif //GOOGOM_TCP_SESSION_H
