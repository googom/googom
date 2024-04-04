//
// Created by root on 3/30/24.
//

#include "topic_definition.h"

#include <utility>

mp::uint128_t TopicDefinition::getAutoIncrementedOffset() {
    mp::uint128_t offset{0};
    if (recentBuffer.empty()) {
        if (topicIo.fileExists(diskFilePath)) {
            /*
            auto data = topicIo.readRecordsFromTail(diskFilePath, 1);
            offset = data.back().getOffset();
            offset++;
             */
            throw std::runtime_error("Topic should be loaded to recent first");
        }
        return offset;
    }
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

    initialLoadBufferFromDisk();
}

void TopicDefinition::initialLoadBufferFromDisk() {
    if (topicIo.fileExists(diskFilePath)) {
        recentBuffer = topicIo.readRecordsFromTail(diskFilePath, bufferSize);
        oldestBuffer = topicIo.readRecordsFromHead(diskFilePath, bufferSize);
    }
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

//TODO this should generate the filename
std::string
TopicDefinition::topicFileNameGenerator(std::string topicName, int partition) {
    throw std::runtime_error("not implemented");
}
