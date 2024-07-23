//
// Created by root on 6/22/24.
//

#include "notification_manager.h"

seastar::future<> NotificationManager::add_subscription(const std::string &topic, TcpSession *session) {
    std::cout << "Added subscription for topic: " << topic << std::endl;
    _subscriptions[topic].insert(session);
    return seastar::make_ready_future<>();
}

seastar::future<> NotificationManager::remove_subscription(const std::string &topic, TcpSession *session) {
    if (_subscriptions.find(topic) != _subscriptions.end()) {
        _subscriptions[topic].erase(session);
        std::cout << "Removed subscription for topic: " << topic << std::endl;
    }
    return seastar::make_ready_future<>();
}

seastar::future<> NotificationManager::notify_subscribers(const std::string &topic, const std::string &message) {
    if (_subscriptions.find(topic) != _subscriptions.end()) {
        std::cout << "Notifying subscribers for topic: " << topic << std::endl;
        for (auto session : _subscriptions[topic]) {
            session->out.write("New message in topic " + topic + ": " + message + "\n").then([session] {
                return session->out.flush();
            }).handle_exception([](std::exception_ptr eptr) {
                try {
                    std::rethrow_exception(eptr);
                } catch (const std::exception &e) {
                    std::cerr << "Error notifying subscriber: " << e.what() << std::endl;
                }
            });
        }
    }
    return seastar::make_ready_future<>();
}
