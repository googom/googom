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


boost::multiprecision::uint128_t TopicManager::getNewestOffsetWithRoundRobin(const std::string &topicName) {
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

    // Since new entries are appended, the last element will have the largest offset
    auto topicPrivateOffsetDefinitionValue = topicPrivateOffsetDefinitionVector.back();
    boost::multiprecision::uint128_t lastOffset = topicPrivateOffsetDefinitionValue.getOffset() + 1;
    topicPrivateOffsetDefinitionValue.setOffset(lastOffset);

    //UPDATE THE OFFSET!!!!
    auto index = topicPrivateOffsetDefinition->searchForIndex(topicPrivateOffsetDefinitionValue);
    topicPrivateOffsetDefinition->update(index, topicPrivateOffsetDefinitionValue);

    // Return the latest offset + 1
    return lastOffset;
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


