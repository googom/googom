//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_IO_H
#define GOOGOM_TOPIC_IO_H

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>

#include <iostream>

#include "../topic_public/topic_structure.h"
#include "../utilities/print_utilities.h"

class TopicIO {
public:
    bool openFile(
      const std::string& filePath,
      std::shared_ptr<arrow::io::ReadableFile>& file);
    // Function to handle reading record batches
    bool readRecordBatches(
      const std::shared_ptr<arrow::ipc::RecordBatchFileReader>& fileReader);
    void writeToDisk(std::string diskFilePath,const TopicStructure& data);
};

#endif // GOOGOM_TOPIC_IO_H
