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

    topicIo.writeToDisk(diskFilePath, data);

    // TODO populate data from disk for recent
    recentBuffer.push_back(data);
    if (recentBuffer.size() > bufferSize) {
        recentBuffer.erase(recentBuffer.begin());
    }
}
const std::string& TopicDefinition::getTopicName() const { return topicName; }
const int TopicDefinition::getPartition() const { return partition; }
const std::vector<TopicStructure>& TopicDefinition::getRecentBuffer() const {
    return recentBuffer;
}
void TopicDefinition::setRecentBuffer(
  const std::vector<TopicStructure>& recentBuffer) {
    TopicDefinition::recentBuffer = recentBuffer;
}
const std::vector<TopicStructure>& TopicDefinition::getOldestBuffer() const {
    return oldestBuffer;
}
void TopicDefinition::setOldestBuffer(
  const std::vector<TopicStructure>& oldestBuffer) {
    TopicDefinition::oldestBuffer = oldestBuffer;
}
const std::string& TopicDefinition::getDiskFilePath() const {
    return diskFilePath;
}
const int TopicDefinition::getBufferSize() const { return bufferSize; }
