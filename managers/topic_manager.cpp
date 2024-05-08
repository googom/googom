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
    } else {
        return true;
    }
}


boost::multiprecision::uint128_t TopicManager::getLatestOffsetWithRoundRobin(const std::string &topicName) {
    auto topicPrivateOffsetDefinition = TopicPrivateOffsetDefinition::getInstance();
    auto topicPrivateOffsetDefinitionVector = topicPrivateOffsetDefinition->searchByCriteriaVectorTypeReturn(topicName,
                                                                                                             nodeId,
                                                                                                             "end");

    if (topicPrivateOffsetDefinitionVector.empty()) {
        createTopic(topicName);
        // Assuming newly created topics start from offset 0, return 1 as the next offset
        return 0;
    }

    // TODO THIS IS STILL WRONG
    // NEEDS TO BE REVISITED ONCE THERE IS A BETTER APPROACH FOR multinode communication!

    // Find the largest offset using std::max_element
    auto maxOffsetIterator = std::max_element(topicPrivateOffsetDefinitionVector.begin(),
                                              topicPrivateOffsetDefinitionVector.end(),
                                              [](const TopicPrivateOffsetStructure &a,
                                                 const TopicPrivateOffsetStructure &b) {
                                                  return a.getOffset() < b.getOffset();
                                              });

    // Check if the iterator is valid and dereference safely
    if (maxOffsetIterator != topicPrivateOffsetDefinitionVector.end()) {
        // Return the largest offset + 1
        auto oldOffset = maxOffsetIterator->getOffset();
        auto newOffset = oldOffset+1;
        TopicPrivateOffsetDefinition::getInstance()->updateOffset(oldOffset, newOffset);

        return newOffset;
    } else {
        // This case shouldn't happen, but for safety, return 1
        return 0;
    }
}


bool TopicManager::createTopic(const std::string &topicName,
                               int partitionCount,
                               int replication,
                               int isrCount) {
    auto topicPrivateOffsetDefinition = TopicPrivateOffsetDefinition::getInstance();

    //TODO check if the topic exists first!!

    auto tsp = Utils::getCurrentMicroseconds();
    TopicPrivateOffsetStructure struct_start(boost::multiprecision::uint128_t(0), tsp, topicName, nodeId,
                                             partitionCount, "start");
    TopicPrivateOffsetStructure struct_end(boost::multiprecision::uint128_t(0), tsp, topicName, nodeId, partitionCount,
                                           "end");
    topicPrivateOffsetDefinition->insert(struct_start);
    topicPrivateOffsetDefinition->insert(struct_end);

    return false;
}


