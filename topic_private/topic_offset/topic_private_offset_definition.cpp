#include "topic_private_offset_definition.h"

#include <algorithm>  // std::find_if
#include <iostream>   // std::cout

// Initialize static members
boost::shared_mutex TopicPrivateOffsetDefinition::mutex_;
std::shared_ptr<TopicPrivateOffsetDefinition> TopicPrivateOffsetDefinition::instance_ = nullptr;

// Method to insert a new TopicPrivateOffsetStructure
void TopicPrivateOffsetDefinition::insert(const TopicPrivateOffsetStructure &new_struct) {
    privateOffsetStructure.push_back(new_struct);
}

std::shared_ptr<TopicPrivateOffsetDefinition> TopicPrivateOffsetDefinition::getInstance() {
    boost::upgrade_lock<boost::shared_mutex> upgradeLock(mutex_);
    if (!instance_) {
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(upgradeLock);
        if (!instance_) {
            instance_ = std::shared_ptr<TopicPrivateOffsetDefinition>(new TopicPrivateOffsetDefinition);
        }
    }
    return instance_;
}

// Method to update an existing TopicPrivateOffsetStructure by index
void TopicPrivateOffsetDefinition::update(int index, const TopicPrivateOffsetStructure &updated_struct) {
    if (index >= 0 && index < static_cast<int>(privateOffsetStructure.size())) {
        privateOffsetStructure[index] = updated_struct;
    } else {
        // Optionally handle the error, maybe throw an exception or log an error
    }
}

// Method to search for a structure by offset and return its index
int TopicPrivateOffsetDefinition::searchByOffset(boost::multiprecision::uint128_t offset) {
    auto it = std::find_if(privateOffsetStructure.begin(), privateOffsetStructure.end(),
                           [&offset](const TopicPrivateOffsetStructure &entry) {
                               return entry.getOffset() == offset;
                           });

    if (it != privateOffsetStructure.end()) {
        return std::distance(privateOffsetStructure.begin(), it);
    }
    return -1; // Not found
}

// Method to search for a structure by specific criteria and return it
TopicPrivateOffsetStructure TopicPrivateOffsetDefinition::searchByCriteriaTypeReturn(const std::string &topic,
                                                                                     const std::string &nodeId,
                                                                                     uint8_t partition,
                                                                                     const std::string &type) {
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

// Method to search for a structure by specific criteria and return its index
int TopicPrivateOffsetDefinition::searchByCriteria(const std::string &topic,
                                                   const std::string &nodeId,
                                                   uint8_t partition,
                                                   const std::string &type) {
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

// Print the details of a single TopicPrivateOffsetStructure
void TopicPrivateOffsetDefinition::printStruct(const TopicPrivateOffsetStructure& p_struct) {
    std::cout << "Offset: " << p_struct.getOffset() << ", "
              << "Timestamp: " << p_struct.getTimestamp() << ", "
              << "Topic: " << p_struct.getTopic() << ", "
              << "Node ID: " << p_struct.getNodeId() << ", "
              << "Partition: " << static_cast<int>(p_struct.getPartition()) << ", "
              << "Type: " << p_struct.getType() << std::endl;
}

// Print the details of a single TopicPrivateOffsetStructure
void TopicPrivateOffsetDefinition::printStruct(int index) {
    if (index >= 0 && index < static_cast<int>(privateOffsetStructure.size())) {
        printStruct(privateOffsetStructure[index]);
    }
}

// Print all TopicPrivateOffsetStructures in the privateOffsetStructure vector
void TopicPrivateOffsetDefinition::printAll() {
    if (privateOffsetStructure.empty()) {
        std::cout << "No entries available." << std::endl;
    } else {
        for (const auto& structure : privateOffsetStructure) {
            printStruct(structure);
        }
    }
}
