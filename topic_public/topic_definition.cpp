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

// Function to print error messages
void TopicDefinition::printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

// Function to open file and handle errors
bool TopicDefinition::openFile(
  const std::string& filePath, std::shared_ptr<arrow::io::ReadableFile>& file) {
    auto status = arrow::io::ReadableFile::Open(
      filePath, arrow::default_memory_pool());
    if (!status.ok()) {
        printError(
          "Error opening Arrow file for reading: "
          + status.status().ToString());
        return false;
    }
    file = std::move(status.ValueOrDie());
    return true;
}

// Function to handle reading record batches
bool TopicDefinition::readRecordBatches(
  const std::shared_ptr<arrow::ipc::RecordBatchFileReader>& fileReader) {
    bool success = true; // Keep track of success
    int i = 0;           // Counter for record batches
    while (true) {
        auto result = fileReader->ReadRecordBatch(i);
        if (!result.ok()) {
            if (result.status().IsIOError()) {
                // Reached end of file
                break;
            } else {
                // Some other error occurred
                printError(
                  "Error reading record batch " + std::to_string(i) + ": "
                  + result.status().ToString());
                success = false; // Update success status
            }
        } else {
            auto batch = result.ValueOrDie();
            std::cout << "Record Batch " << i << ":\n"
                      << batch->ToString() << std::endl;
        }
        ++i; // Move to the next record batch
    }
    return success; // Return overall success status
}

void TopicDefinition::writeToDisk(const TopicStructure& data) {
    // Open file in append mode or create a new one if it doesn't exist
    auto file_result = arrow::io::FileOutputStream::Open(diskFilePath, true);
    if (!file_result.ok()) {
        std::cerr << "Error opening file: " << file_result.status()
                  << std::endl;
        return;
    }
    auto file_writer = std::move(*file_result);

    // Create a RecordBatch from the data
    auto pool = arrow::default_memory_pool();

    // TODO missing items

    arrow::StringBuilder offsetBuilder(pool), timestampBuilder(pool),
      headersBuilder(pool);
    arrow::BinaryBuilder valueBuilder(pool);

    offsetBuilder.Append(data.getOffset().str());
    timestampBuilder.Append(data.getTimestamp());
    headersBuilder.Append(data.getHeaders());
    valueBuilder.Append(data.getValue().data(), data.getValue().size());

    std::shared_ptr<arrow::Array> offsetArray, timestampArray, topicNameArray,
      headersArray, valueArray;
    offsetBuilder.Finish(&offsetArray);
    timestampBuilder.Finish(&timestampArray);
    headersBuilder.Finish(&headersArray);
    valueBuilder.Finish(&valueArray);

    auto schema = arrow::schema(
      {arrow::field("Offset", arrow::utf8()),
       arrow::field("Timestamp", arrow::utf8()),
       arrow::field("Headers", arrow::utf8()),
       arrow::field("Value", arrow::binary())});

    // Create a RecordBatch from the arrays
    auto batch = arrow::RecordBatch::Make(
      schema, 1, {offsetArray, timestampArray, headersArray, valueArray});

    // Write the RecordBatch to the file
    auto writer_result = arrow::ipc::MakeFileWriter(file_writer.get(), schema);
    if (!writer_result.ok()) {
        std::cerr << "Error creating file writer: " << writer_result.status()
                  << std::endl;
        return;
    }
    auto writer = std::move(*writer_result);

    if (!writer->WriteRecordBatch(*batch).ok()) {
        std::cerr << "Failed to write RecordBatch to file" << std::endl;
        return;
    }

    // Close the file writer
    if (!writer->Close().ok()) {
        std::cerr << "Failed to close file writer" << std::endl;
        return;
    }
}

void TopicDefinition::printDiskData() {
    // Print data from the disk file
    std::cout << "Data from disk:" << std::endl;
    std::shared_ptr<arrow::io::ReadableFile> file;
    if (!openFile(diskFilePath, file)) {
        return; // Handle error accordingly
    }

    auto fileReaderResult = arrow::ipc::RecordBatchFileReader::Open(file);
    if (!fileReaderResult.ok()) {
        printError(
          "Error opening Arrow file for reading: "
          + fileReaderResult.status().ToString());
        return; // Handle error accordingly
    }
    auto fileReader = *fileReaderResult;

    std::cout << "Schema:\n" << fileReader->schema()->ToString() << std::endl;

    if (!readRecordBatches(fileReader)) {
        return; // Handle error accordingly
    }

    std::cout << std::resetiosflags(std::ios::basefield);
}

void TopicDefinition::printBuffer(
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

void TopicDefinition::printAllData() {
    printBuffer(recentBuffer, "recentBuffer");

    // printDiskData();

    printBuffer(oldestBuffer, "oldestBuffer");
}
