//
// Created by root on 5/3/24.
//

#ifndef GOOGOM_TOPIC_MANAGER_H
#define GOOGOM_TOPIC_MANAGER_H

#include "../topics/topic_private/topic_offset/topic_private_offset_structure.h"
#include "../topics/topic_private/topic_offset/topic_private_offset_definition.h"
#include "config_manager.h"

class TopicManager {
    static std::string nodeId;
public:

    static boost::multiprecision::uint128_t getLatestOffsetWithRoundRobin(std::string &topicName);

    static bool createTopic(std::string &topicName, int partitionCount=ConfigManager::getDefaultPartitionCount(), int replication=ConfigManager::getDefaultReplicationCount(), int isrCount=ConfigManager::getDefaultIsrCount()) ;
};


#endif //GOOGOM_TOPIC_MANAGER_H