//
// Created by root on 5/3/24.
//

#include "topic_manager.h"

std::string TopicManager::nodeId = ConfigManager::getNodeId();

boost::multiprecision::uint128_t TopicManager::getLatestOffsetWithRoundRobin(std::string &topicName) {

    auto topicPrivateOffsetDefinition = TopicPrivateOffsetDefinition::getInstance();
    auto topicPrivateOffsetDefinitionVector = topicPrivateOffsetDefinition->searchByCriteriaVectorTypeReturn(topicName, nodeId, "end");

    if (topicPrivateOffsetDefinitionVector.empty()) {
        // Handle case when no offsets are found for the topic
        return -1; // Or any other appropriate error code
    }

    // Use std::min_element to find the smallest offset count
    auto minOffsetIterator = std::min_element(topicPrivateOffsetDefinitionVector.begin(), topicPrivateOffsetDefinitionVector.end(),
                                              [](const TopicPrivateOffsetStructure& a, const TopicPrivateOffsetStructure& b) {
                                                  return a.getOffset() < b.getOffset();
                                              });

    // Dereference the iterator to get the smallest offset count
    boost::multiprecision::uint128_t minOffset = minOffsetIterator->getOffset();

    return minOffset;
}

bool TopicManager::createTopic(std::string &topicName, int partitionCount=ConfigManager::getDefaultPartitionCount(), int replication=ConfigManager::getDefaultReplicationCount(), int isrCount=ConfigManager::getDefaultIsrCount()) {
    auto topicPrivateOffsetDefinition = TopicPrivateOffsetDefinition::getInstance();


    return false;
}


