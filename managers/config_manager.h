//
// Created by root on 5/4/24.
//

#ifndef GOOGOM_CONFIG_MANAGER_H
#define GOOGOM_CONFIG_MANAGER_H

#include <iostream>

// Configuration should work both ways.
// On the first load of application, data should be loaded to the topic from the file
// Once there is a change coming from the application via API, these changes should be written to all instances

class ConfigManager{
    static std::string nodeId;
    static int defaultPartitionCount;
    static int defaultReplicationCount;
    static int defaultISRCount;


public:
    static std::string &getNodeId();

    static void setNodeId(const std::string &nodeId);

    static int getDefaultPartitionCount();

    static void setDefaultPartitionCount(int defaultPartitionCount);

    static int getDefaultReplicationCount();

    static void setDefaultReplicationCount(int defaultReplicationCount);

    static int getDefaultIsrCount();

    static void setDefaultIsrCount(int defaultIsrCount);

};

#endif //GOOGOM_CONFIG_MANAGER_H
