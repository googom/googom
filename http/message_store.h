//
// Created by root on 4/29/24.
//

#ifndef GOOGOM_MESSAGE_STORE_H
#define GOOGOM_MESSAGE_STORE_H


#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/future.hh>
#include <seastar/http/httpd.hh>
#include <seastar/net/api.hh>
#include <seastar/core/sleep.hh>
#include <iostream>
#include <unordered_map>
#include <optional>

// Message Store for REST server
class message_store {
public:
    std::unordered_map<int, std::string> messages;
    int next_id = 0;

    int store_message(const std::string& msg) {
        int id = next_id++;
        messages[id] = msg;
        return id;
    }

    // Modify this function to return a future that wraps an optional string
    seastar::future<std::optional<std::string>> get_message(int id) {
        auto it = messages.find(id);
        if (it == messages.end()) {
            return seastar::make_ready_future<std::optional<std::string>>(std::nullopt);
        }
        return seastar::make_ready_future<std::optional<std::string>>(it->second);
    }

    bool update_message(int id, const std::string& msg) {
        if (messages.count(id) == 0) {
            return false;
        }
        messages[id] = msg;
        return true;
    }

    bool delete_message(int id) {
        return messages.erase(id) > 0;
    }
};


#endif //GOOGOM_MESSAGE_STORE_H
