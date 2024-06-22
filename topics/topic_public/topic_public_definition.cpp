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

boost::multiprecision::uint128_t TopicPublicDefinition::insert(const TopicPublicMessage& data) {


    auto offset=getAutoIncrementedOffset();

    TopicPublicStructure topicPublicStructure=TopicPublicStructure(
                                                  offset,
                                                  Utils::getCurrentMicroseconds(),
                                                  data.getKeys(),
                                                  data.getHeaders(),
                                                  0,
                                                  data.getValue()
                                                  );

    //
    //Utils::getCurrentMicroseconds(),

    //data.setOffset(getAutoIncrementedOffset());

    topicIo.writeToDisk(diskFilePath, topicPublicStructure);

    // TODO populate data from disk for recent
    recentBuffer.push_back(topicPublicStructure);
    if (recentBuffer.size() > bufferSize) {
        recentBuffer.erase(recentBuffer.begin());
    }

    return offset;
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


TopicPublicStructure TopicPublicDefinition::searchByOffset(const boost::multiprecision::uint128_t& offset) {
    // Search in recentBuffer
    for (const auto& record : recentBuffer) {
        if (record.getOffset() == offset) {
            return record;
        }
    }

    // Search in oldestBuffer
    for (const auto& record : oldestBuffer) {
        if (record.getOffset() == offset) {
            return record;
        }
    }

    // If not found, search in Arrow archive files
    if (topicIo.fileExists(diskFilePath)) {
        return topicIo.searchInArrowFiles(diskFilePath, offset);
    }

    throw std::runtime_error("Record not found");
}

bool TopicPublicDefinition::deleteByOffset(const boost::multiprecision::uint128_t& offset) {
    // Delete from recentBuffer
    auto it1 = std::find_if(recentBuffer.begin(), recentBuffer.end(),
                           [offset](const TopicPublicStructure& record) {
                               return record.getOffset() == offset;
                           });

    if (it1 != recentBuffer.end()) {
        recentBuffer.erase(it1);
        //return true;
    }

    // Delete from recentBuffer
    auto it2 = std::find_if(oldestBuffer.begin(), oldestBuffer.end(),
                            [offset](const TopicPublicStructure& record) {
                                return record.getOffset() == offset;
                            });

    if (it2 != oldestBuffer.end()) {
        oldestBuffer.erase(it2);
        //return true;
    }

    // If not found, delete from Arrow archive files
    if (topicIo.fileExists(diskFilePath)) {
        return topicIo.deleteFromArrowFiles(diskFilePath, offset);
    }

    return false; // Record not found
}
