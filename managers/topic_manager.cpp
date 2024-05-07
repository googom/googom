//
// Created by root on 5/3/24.
//

#include "topic_manager.h"

std::string TopicManager::nodeId = ConfigManager::getInstance()->getNodeId();

bool TopicManager::checkTopicExists(const std::string &topicName) {
    auto topicPrivateOffsetDefinition = TopicPrivateOffsetDefinition::getInstance();
    auto topicPrivateOffsetDefinitionVector = topicPrivateOffsetDefinition->searchByNameTypeReturn(topicName);

    if (topicPrivateOffsetDefinitionVector.empty()) {
        // Handle case when no offsets are found for the topic
        return false; // Or any other appropriate error code
    } else{
        return true;
    }
}

boost::multiprecision::uint128_t TopicManager::getLatestOffsetWithRoundRobin(const std::string &topicName) {

    auto topicPrivateOffsetDefinition = TopicPrivateOffsetDefinition::getInstance();
    auto topicPrivateOffsetDefinitionVector = topicPrivateOffsetDefinition->searchByCriteriaVectorTypeReturn(topicName, nodeId, "end");

    if (topicPrivateOffsetDefinitionVector.empty()) {
        // Handle case when no offsets are found for the topic
        return -1; // Or any other appropriate error code
    }

    // TODO THIS IS STILL WRONG
    // NEEDS TO BE REVISITED ONCE THERE IS A BETTER APPROACH FOR multinode communication!

    // Use std::min_element to find the smallest offset count
    auto minOffsetIterator = std::min_element(topicPrivateOffsetDefinitionVector.begin(), topicPrivateOffsetDefinitionVector.end(),
                                              [](const TopicPrivateOffsetStructure& a, const TopicPrivateOffsetStructure& b) {
                                                  return a.getOffset() < b.getOffset();
                                              });

    // Dereference the iterator to get the smallest offset count
    boost::multiprecision::uint128_t minOffset = minOffsetIterator->getOffset();

    return minOffset;
}

bool TopicManager::createTopic(const std::string &topicName, int partitionCount=ConfigManager::getInstance()->getDefaultPartitionCount(), int replication=ConfigManager::getInstance()->getDefaultReplicationCount(), int isrCount=ConfigManager::getInstance()->getDefaultIsrCount()) {
    auto topicPrivateOffsetDefinition = TopicPrivateOffsetDefinition::getInstance();
    auto tsp=Utils::getCurrentMicroseconds();
    TopicPrivateOffsetStructure struct_start(boost::multiprecision::uint128_t(0), tsp, topicName, nodeId, partitionCount,"start");
    TopicPrivateOffsetStructure struct_end(boost::multiprecision::uint128_t(0), tsp, topicName, nodeId, partitionCount,"end");
    topicPrivateOffsetDefinition->insert(struct_start);
    topicPrivateOffsetDefinition->insert(struct_end);

    return false;
}


