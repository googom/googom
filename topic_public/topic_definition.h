//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_DEFINITION_H
#define GOOGOM_TOPIC_DEFINITION_H

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
    static void printError(const std::string& message);
    bool openFile(
      const std::string& filePath,
      std::shared_ptr<arrow::io::ReadableFile>& file);
    // Function to handle reading record batches
    bool readRecordBatches(
      const std::shared_ptr<arrow::ipc::RecordBatchFileReader>& fileReader);
    void writeToDisk(const TopicStructure& data);
    void printDiskData();
    static void printBuffer(
      const std::vector<TopicStructure>& buffer, const std::string& name);

public:
    TopicDefinition(std::string topicName, int partition, int bufferSize, std::string diskFilePath);
    void insert(TopicStructure& data);
    void printAllData();
};

#endif // GOOGOM_TOPIC_DEFINITION_H
