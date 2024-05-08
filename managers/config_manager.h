//
// Created by root on 5/4/24.
//

#ifndef GOOGOM_CONFIG_MANAGER_H
#define GOOGOM_CONFIG_MANAGER_H

// Configuration should work both ways.
// On the first load of application, data should be loaded to the topic from the file
// Once there is a change coming from the application via API, these changes should be written to all instances

#include <iostream>
#include <fstream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <sstream>

class ConfigManager {
    static ConfigManager* instance;
    std::string nodeId;
    int defaultPartitionCount;
    int defaultReplicationCount;
    int defaultISRCount;
    int webPort;
    int tcpPort;

    ConfigManager(); // Private constructor for singleton

public:
    // Disable copy constructor and assignment operator
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    static ConfigManager* getInstance(); // Singleton access method

    // Getters and setters
    std::string getNodeId() const;
    void setNodeId(const std::string& nodeId);

    int getDefaultPartitionCount() const;
    void setDefaultPartitionCount(int defaultPartitionCount);

    int getDefaultReplicationCount() const;
    void setDefaultReplicationCount(int defaultReplicationCount);

    int getDefaultIsrCount() const;
    void setDefaultIsrCount(int defaultIsrCount);

    int getWebPort() const;

    void setWebPort(int webPort);

    int getTcpPort() const;

    void setTcpPort(int tcpPort);

    // File I/O
    void loadConfig(const std::string& filename);
    void saveConfig(const std::string& filename);
};

#endif //GOOGOM_CONFIG_MANAGER_H
