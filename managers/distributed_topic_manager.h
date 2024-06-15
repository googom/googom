//
// Created by root on 6/15/24.
//

#ifndef GOOGOM_DISTRIBUTED_TOPIC_MANAGER_H
#define GOOGOM_DISTRIBUTED_TOPIC_MANAGER_H

#include <seastar/core/sharded.hh>
#include <unordered_map>
#include <memory>
#include "../topics/topic_public/topic_public_definition.h"


class DistributedTopicManager {
public:
    seastar::future<> start();

    seastar::future<> stop();

    seastar::future<std::shared_ptr<TopicPublicDefinition>> getOrCreateTopicPublicDefinition(
            const std::string &topicName,
            int partition,
            int bufferSize,
            const std::string &diskFilePath
    );

private:
    std::string generateKey(const std::string &topicName, int partition) const;

    std::unordered_map<std::string, std::shared_ptr<TopicPublicDefinition>> topics_;
};


#endif //GOOGOM_DISTRIBUTED_TOPIC_MANAGER_H
