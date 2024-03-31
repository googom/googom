//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_DEBUGGING_H
#define GOOGOM_TOPIC_DEBUGGING_H

#include "../topic_io/topic_io.h"
#include "../topic_public/topic_structure.h"
#include "../utilities/print_utilities.h"

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>

#include <iostream>

class TopicDebugging {
    TopicIO topicIo = TopicIO{};

public:
    TopicDebugging();
    void printDiskData(std::string diskFilePath);
    static void printBuffer(
      const std::vector<TopicStructure>& buffer, const std::string& name);
};

#endif // GOOGOM_TOPIC_DEBUGGING_H
