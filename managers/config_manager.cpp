//
// Created by root on 5/4/24.
//

#include "config_manager.h"

ConfigManager* ConfigManager::instance = nullptr;

ConfigManager::ConfigManager() : nodeId(""), defaultPartitionCount(0), defaultReplicationCount(0), defaultISRCount(0) {}

ConfigManager* ConfigManager::getInstance() {
    if (instance == nullptr) {
        instance = new ConfigManager();
    }
    return instance;
}

std::string ConfigManager::getNodeId() const {
    return nodeId;
}

void ConfigManager::setNodeId(const std::string& id) {
    nodeId = id;
}

int ConfigManager::getDefaultPartitionCount() const {
    return defaultPartitionCount;
}

void ConfigManager::setDefaultPartitionCount(int count) {
    defaultPartitionCount = count;
}

int ConfigManager::getDefaultReplicationCount() const {
    return defaultReplicationCount;
}

void ConfigManager::setDefaultReplicationCount(int count) {
    defaultReplicationCount = count;
}

int ConfigManager::getDefaultIsrCount() const {
    return defaultISRCount;
}

void ConfigManager::setDefaultIsrCount(int count) {
    defaultISRCount = count;
}

void ConfigManager::loadConfig(const std::string& filename) {
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filename, pt); // Or use read_ini for INI files

        nodeId = pt.get<std::string>("Configuration.nodeId");
        defaultPartitionCount = pt.get<int>("Configuration.defaultPartitionCount");
        defaultReplicationCount = pt.get<int>("Configuration.defaultReplicationCount");
        defaultISRCount = pt.get<int>("Configuration.defaultISRCount");

        webPort=pt.get<int>("Web.port");
        tcpPort=pt.get<int>("Tcp.port");

    } catch (const boost::property_tree::ptree_error &e) {
        std::cerr << "Error reading config file: " << e.what() << std::endl;
    }
}

void ConfigManager::saveConfig(const std::string& filename) {
    try {
        boost::property_tree::ptree pt;
        pt.put("Configuration.nodeId", nodeId);
        pt.put("Configuration.defaultPartitionCount", defaultPartitionCount);
        pt.put("Configuration.defaultReplicationCount", defaultReplicationCount);
        pt.put("Configuration.defaultISRCount", defaultISRCount);


        pt.put("Web.port", webPort);
        pt.put("Tcp.port", tcpPort);

        boost::property_tree::write_json(filename, pt);
    } catch (const boost::property_tree::json_parser_error &e) {
        std::cerr << "Error writing config file: " << e.what() << std::endl;
    }
}

int ConfigManager::getWebPort() const {
    return webPort;
}

void ConfigManager::setWebPort(int webPort) {
    ConfigManager::webPort = webPort;
}

int ConfigManager::getTcpPort() const {
    return tcpPort;
}

void ConfigManager::setTcpPort(int tcpPort) {
    ConfigManager::tcpPort = tcpPort;
}
