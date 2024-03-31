//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_DEBUGGING_H
#define GOOGOM_TOPIC_DEBUGGING_H

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>

#include <iostream>

#include "../topic_public/topic_structure.h"

class TopicDebugging {


    void printDiskData();
    static void printBuffer(
      const std::vector<TopicStructure>& buffer, const std::string& name);

public:
    void printAllData();
};

#endif // GOOGOM_TOPIC_DEBUGGING_H
