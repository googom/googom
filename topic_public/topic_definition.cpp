//
// Created by root on 3/30/24.
//

#include "topic_definition.h"

#include <utility>

mp::uint128_t TopicDefinition::getAutoIncrementedOffset() {
    mp::uint128_t offset{0};
    if (recentBuffer.empty()) return offset;
    // TODO read from disk!!
    offset = recentBuffer.back().getOffset();
    return ++offset;
}





TopicDefinition::TopicDefinition(
  std::string topicName,
  int partition,
  int bufferSize,
  std::string diskFilePath)
  : topicName(std::move(topicName))
  , partition(partition)
  , bufferSize(bufferSize)
  , diskFilePath(std::move(diskFilePath)) {
    recentBuffer.reserve(bufferSize);
    oldestBuffer.reserve(bufferSize);
}

void TopicDefinition::insert(TopicStructure& data) {
    data.setOffset(getAutoIncrementedOffset());

    writeToDisk(data);

    // TODO populate data from disk for recent
    recentBuffer.push_back(data);
    if (recentBuffer.size() > bufferSize) {
        recentBuffer.erase(recentBuffer.begin());
    }
}


