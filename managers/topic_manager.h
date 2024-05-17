//
// Created by root on 5/3/24.
//

#ifndef GOOGOM_TOPIC_MANAGER_H
#define GOOGOM_TOPIC_MANAGER_H

#include "../topics/topic_private/topic_offset/topic_private_offset_structure.h"
#include "../topics/topic_private/topic_offset/topic_private_offset_definition.h"
#include "config_manager.h"
#include "../utilities/utils.h"

class TopicManager {
    static std::string nodeId;
public:

    static boost::multiprecision::uint128_t getNewestOffsetWithRoundRobin(const std::string &topicName);

    static bool createTopic(const std::string &topicName,
                                   int partitionCount = ConfigManager::getInstance()->getDefaultPartitionCount(),
                                   int replication = ConfigManager::getInstance()->getDefaultReplicationCount(),
                                   int isrCount = ConfigManager::getInstance()->getDefaultIsrCount());

    static bool checkTopicExists(const std::string &topicName);
};


#endif //GOOGOM_TOPIC_MANAGER_H
