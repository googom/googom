//
// Created by root on 8/13/24.
//

#ifndef GOOGOM_SUBSCRIPTION_H
#define GOOGOM_SUBSCRIPTION_H

#include <string>



class Subscription {
public:
    std::string topicName;
    std::string subscriptionId;

    bool operator==(const Subscription& other) const {
        return subscriptionId == other.subscriptionId && topicName == other.topicName;
    }

    bool operator!=(const Subscription& other) const {
        return !(*this == other);
    }
};

#endif // GOOGOM_SUBSCRIPTION_H

