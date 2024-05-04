#include "topic_private_offset_definition.h"

#include <algorithm>  // for std::find_if
#include <iostream>   // for std::cout
#include <boost/thread/locks.hpp>  // for boost::unique_lock and boost::shared_lock

// Initialize static members
boost::shared_mutex TopicPrivateOffsetDefinition::mutex_;
std::shared_ptr<TopicPrivateOffsetDefinition> TopicPrivateOffsetDefinition::instance_ = nullptr;

// Singleton pattern implementation
std::shared_ptr<TopicPrivateOffsetDefinition> TopicPrivateOffsetDefinition::getInstance() {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    if (!instance_) {
        instance_ = std::shared_ptr<TopicPrivateOffsetDefinition>(new TopicPrivateOffsetDefinition);
    }
    return instance_;
}

// Insert a new structure - Write operation
void TopicPrivateOffsetDefinition::insert(const TopicPrivateOffsetStructure &new_struct) {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    privateOffsetStructure.push_back(new_struct);
}

// Update an existing structure by index - Write operation
void TopicPrivateOffsetDefinition::update(int index, const TopicPrivateOffsetStructure &updated_struct) {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    if (index >= 0 && index < static_cast<int>(privateOffsetStructure.size())) {
        privateOffsetStructure[index] = updated_struct;
    } else {
        // Optionally handle the error, maybe throw an exception or log an error
    }
}

// Search for a structure by offset and return its index - Read operation
int TopicPrivateOffsetDefinition::searchByOffset(boost::multiprecision::uint128_t offset) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto it = std::find_if(privateOffsetStructure.begin(), privateOffsetStructure.end(),
                           [&offset](const TopicPrivateOffsetStructure &entry) {
                               return entry.getOffset() == offset;
                           });

    if (it != privateOffsetStructure.end()) {
        return std::distance(privateOffsetStructure.begin(), it);
    }
    return -1; // Not found
}

// Search for a structure by specific criteria and return it - Read operation
TopicPrivateOffsetStructure TopicPrivateOffsetDefinition::searchByCriteriaTypeReturn(const std::string &topic,
                                                                                     const std::string &nodeId,
                                                                                     uint8_t partition,
                                                                                     const std::string &type) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto it = std::find_if(privateOffsetStructure.begin(), privateOffsetStructure.end(),
                           [&topic, &nodeId, partition, &type](const TopicPrivateOffsetStructure &entry) {
                               return entry.getTopic() == topic && entry.getNodeId() == nodeId &&
                                      entry.getPartition() == partition && entry.getType() == type;
                           });

    if (it != privateOffsetStructure.end()) {
        return *it;
    }

    return {};  // or throw an exception based on your error handling strategy
}

int TopicPrivateOffsetDefinition::searchByCriteria(const std::string &topic, const std::string &nodeId, uint8_t partition, const std::string &type) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto it = std::find_if(privateOffsetStructure.begin(), privateOffsetStructure.end(),
                           [&topic, &nodeId, partition, &type](const TopicPrivateOffsetStructure &entry) {
                               return entry.getTopic() == topic && entry.getNodeId() == nodeId &&
                                      entry.getPartition() == partition && entry.getType() == type;
                           });

    if (it != privateOffsetStructure.end()) {
        return std::distance(privateOffsetStructure.begin(), it);
    }
    return -1; // Not found
}


// Print all TopicPrivateOffsetStructures in the privateOffsetStructure vector - Read operation
void TopicPrivateOffsetDefinition::printAll() {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    if (privateOffsetStructure.empty()) {
        std::cout << "No entries available." << std::endl;
    } else {
        for (const auto& structure : privateOffsetStructure) {
            printStruct(structure);
        }
    }
}

// Print the details of a single TopicPrivateOffsetStructure - Read operation
void TopicPrivateOffsetDefinition::printStruct(const TopicPrivateOffsetStructure& p_struct) {
    std::cout << "Offset: " << p_struct.getOffset() << ", "
              << "Timestamp: " << p_struct.getTimestamp() << ", "
              << "Topic: " << p_struct.getTopic() << ", "
              << "Node ID: " << p_struct.getNodeId() << ", "
              << "Partition: " << static_cast<int>(p_struct.getPartition()) << ", "
              << "Type: " << p_struct.getType() << std::endl;
}

// Print the details of a single TopicPrivateOffsetStructure based on index - Read operation
void TopicPrivateOffsetDefinition::printStruct(int index) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    if (index >= 0 && index < static_cast<int>(privateOffsetStructure.size())) {
        printStruct(privateOffsetStructure[index]);
    }
}

std::vector<TopicPrivateOffsetStructure>
TopicPrivateOffsetDefinition::searchByCriteriaVectorTypeReturn(const std::string &topic, const std::string &nodeId,const std::string &type) {

    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    std::vector<TopicPrivateOffsetStructure> result;

    auto it = std::find_if(privateOffsetStructure.begin(), privateOffsetStructure.end(),
                           [&topic, &nodeId, &type](const TopicPrivateOffsetStructure &entry) {
                               return entry.getTopic() == topic && entry.getNodeId() == nodeId && entry.getType() == type;
                           });

    while (it != privateOffsetStructure.end()) {
        result.push_back(*it);
        it = std::find_if(++it, privateOffsetStructure.end(),
                          [&topic, &type](const TopicPrivateOffsetStructure &entry) {
                              return entry.getTopic() == topic && entry.getType() == type;
                          });
    }

    return result;
}
