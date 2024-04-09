//
// Created by root on 3/30/24.
//

#include "topic_public_definition.h"

#include <utility>

boost::multiprecision::uint128_t TopicPublicDefinition::getAutoIncrementedOffset() {
    boost::multiprecision::uint128_t offset{0};
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

TopicPublicDefinition::TopicPublicDefinition(
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

void TopicPublicDefinition::initialLoadBufferFromDisk() {
    if (topicIo.fileExists(diskFilePath)) {
        recentBuffer = topicIo.readRecordsFromTail(diskFilePath, bufferSize);
        oldestBuffer = topicIo.readRecordsFromHead(diskFilePath, bufferSize);
    }
}

void TopicPublicDefinition::insert(const TopicPublicStructure& data) {
    data.setOffset(getAutoIncrementedOffset());

    topicIo.writeToDisk(diskFilePath, data);

    // TODO populate data from disk for recent
    recentBuffer.push_back(data);
    if (recentBuffer.size() > bufferSize) {
        recentBuffer.erase(recentBuffer.begin());
    }
}


const std::string& TopicPublicDefinition::getTopicName() const { return topicName; }
const int TopicPublicDefinition::getPartition() const { return partition; }
const std::vector<TopicPublicStructure>&
TopicPublicDefinition::getRecentBuffer() const {
    return recentBuffer;
}
void TopicPublicDefinition::setRecentBuffer(
  const std::vector<TopicPublicStructure>& recentBuffer) {
    TopicPublicDefinition::recentBuffer = recentBuffer;
}
const std::vector<TopicPublicStructure>&
TopicPublicDefinition::getOldestBuffer() const {
    return oldestBuffer;
}
void TopicPublicDefinition::setOldestBuffer(
  const std::vector<TopicPublicStructure>& oldestBuffer) {
    TopicPublicDefinition::oldestBuffer = oldestBuffer;
}
const std::string& TopicPublicDefinition::getDiskFilePath() const {
    return diskFilePath;
}
const int TopicPublicDefinition::getBufferSize() const { return bufferSize; }

//TODO this should generate the filename
std::string TopicPublicDefinition::topicFileNameGenerator(std::string topicName, int partition) {
    throw std::runtime_error("not implemented");
}
