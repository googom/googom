//
// Created by root on 6/22/24.
//

#ifndef GOOGOM_NOTIFICATION_MANAGER_H
#define GOOGOM_NOTIFICATION_MANAGER_H

#include "../communication/tcp/tcp_session.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <seastar/core/sharded.hh>

class NotificationManager {
public:
    seastar::future<> add_subscription(const std::string &topic, TcpSession *session);
    seastar::future<> remove_subscription(const std::string &topic, TcpSession *session);
    seastar::future<> notify_subscribers(const std::string &topic, const std::string &message);

private:
    std::unordered_map<std::string, std::unordered_set<TcpSession*>> _subscriptions;
};

extern seastar::sharded<NotificationManager> notificationManager;

#endif //GOOGOM_NOTIFICATION_MANAGER_H
