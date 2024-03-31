//
// Created by root on 3/31/24.
//

#include "topic_io.h"

// Function to open file and handle errors
bool TopicIO::openFile(
  const std::string& filePath, std::shared_ptr<arrow::io::ReadableFile>& file) {
    auto status = arrow::io::ReadableFile::Open(
      filePath, arrow::default_memory_pool());
    if (!status.ok()) {
        PrintUtilities::printError(
          "Error opening Arrow file for reading: "
          + status.status().ToString());
        return false;
    }
    file = std::move(status.ValueOrDie());
    return true;
}

// Function to handle reading record batches
bool TopicIO::readRecordBatches(
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
                PrintUtilities::printError(
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

void TopicIO::writeToDisk(std::string diskFilePath,const TopicStructure& data) {
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