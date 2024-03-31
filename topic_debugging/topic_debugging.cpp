//
// Created by root on 3/31/24.
//

#include "topic_debugging.h"

void TopicDebugging::printDiskData(std::string diskFilePath) {
    TopicIO topicIo = TopicIO();

    // Print data from the disk file
    std::cout << "Data from disk:" << std::endl;
    std::shared_ptr<arrow::io::ReadableFile> file;
    if (!topicIo.openFile(diskFilePath, file)) {
        return; // Handle error accordingly
    }

    auto fileReaderResult = arrow::ipc::RecordBatchFileReader::Open(file);
    if (!fileReaderResult.ok()) {
        PrintUtilities::printError(
          "Error opening Arrow file for reading: "
          + fileReaderResult.status().ToString());
        return; // Handle error accordingly
    }
    auto fileReader = *fileReaderResult;

    std::cout << "Schema:\n" << fileReader->schema()->ToString() << std::endl;

    if (!topicIo.readRecordBatches(fileReader)) {
        return; // Handle error accordingly
    }

    std::cout << std::resetiosflags(std::ios::basefield);
}

void TopicDebugging::printBuffer(
  const std::vector<TopicStructure>& buffer,
  const std::string&
    name) { // Print data from the recentBuffer buffer (in-memory)
    std::cout << "In-memory data " << name << " :" << std::endl;
    for (const auto& data : buffer) {
        std::cout << "Offset: " << to_string(data.getOffset())
                  << ", Timestamp: " << data.getTimestamp()
                  << ", Headers: " << data.getHeaders() << ", Value: ";
        for (const auto& byte : data.getValue()) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(byte);
        }
        std::cout << std::endl;
    }

    std::cout << std::resetiosflags(std::ios::basefield);
}
TopicDebugging::TopicDebugging() {}
