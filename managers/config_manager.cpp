//
// Created by root on 5/4/24.
//

#include "config_manager.h"

std::string ConfigManager::nodeId;

std::string &ConfigManager::getNodeId() {
    return nodeId;
}

void ConfigManager::setNodeId(const std::string &nodeId) {
    ConfigManager::nodeId = nodeId;
}

int ConfigManager::getDefaultPartitionCount() {
    return defaultPartitionCount;
}

void ConfigManager::setDefaultPartitionCount(int defaultPartitionCount) {
    ConfigManager::defaultPartitionCount = defaultPartitionCount;
}

int ConfigManager::getDefaultReplicationCount() {
    return defaultReplicationCount;
}

void ConfigManager::setDefaultReplicationCount(int defaultReplicationCount) {
    ConfigManager::defaultReplicationCount = defaultReplicationCount;
}

int ConfigManager::getDefaultIsrCount() {
    return defaultISRCount;
}

void ConfigManager::setDefaultIsrCount(int defaultIsrCount) {
    defaultISRCount = defaultIsrCount;
}
