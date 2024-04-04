//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_DEFINITION_H
#define GOOGOM_TOPIC_DEFINITION_H

#include "../topic_io/topic_io.h"
#include "topic_structure.h"

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>

#include <iostream>

class TopicDefinition {
    const std::string topicName;
    const int partition;

    std::vector<TopicStructure> recentBuffer{};
    std::vector<TopicStructure> oldestBuffer{};
    const std::string diskFilePath;
    const int bufferSize;

    mp::uint128_t getAutoIncrementedOffset();

    TopicIO topicIo = TopicIO();

    void initialLoadBufferFromDisk();

public:
    TopicDefinition(
      std::string topicName,
      int partition,
      int bufferSize,
      std::string diskFilePath);
    void insert(TopicStructure& data);


    std::string topicFileNameGenerator(std::string topicName, int partition);

    const std::string& getTopicName() const;
    const int getPartition() const;
    const std::vector<TopicStructure>& getRecentBuffer() const;
    void setRecentBuffer(const std::vector<TopicStructure>& recentBuffer);
    const std::vector<TopicStructure>& getOldestBuffer() const;
    void setOldestBuffer(const std::vector<TopicStructure>& oldestBuffer);
    const std::string& getDiskFilePath() const;
    const int getBufferSize() const;


};

#endif // GOOGOM_TOPIC_DEFINITION_H
