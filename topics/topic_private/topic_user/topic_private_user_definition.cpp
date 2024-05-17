//
// Created by root on 5/8/24.
//

#include "topic_private_user_definition.h"

#include <algorithm>  // for std::find_if
#include <iostream>   // for std::cout
#include <boost/thread/locks.hpp>  // for boost::unique_lock and boost::shared_lock
#include "../../../utilities/utils.h"

// Initialize static members
boost::shared_mutex TopicPrivateUserDefinition::mutex_;
std::shared_ptr<TopicPrivateUserDefinition> TopicPrivateUserDefinition::instance_ = nullptr;

// Singleton pattern implementation
std::shared_ptr<TopicPrivateUserDefinition> TopicPrivateUserDefinition::getInstance() {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    if (!instance_) {
        instance_ = std::shared_ptr<TopicPrivateUserDefinition>(new TopicPrivateUserDefinition);
    }
    return instance_;
}

// Insert a new structure - Write operation
TopicPrivateUserStructure TopicPrivateUserDefinition::insert(const std::string &username, const std::string &password,const std::string &email) {


    TopicPrivateUserStructure new_struct;
    new_struct.setTimestamp(Utils::getCurrentMicroseconds());
    boost::multiprecision::uint128_t offset=TopicManager::getNewestOffsetWithRoundRobin("__user");
    new_struct.setOffset(offset);
    //TODO check if user exists
    new_struct.setUsername(username);
    //TODO PASSWORD MD5
    new_struct.setPassword(password);
    new_struct.setEmail(email);
    new_struct.setDeleted(false);
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    privateUserStructure.push_back(new_struct);
    return new_struct;
}

// Update an existing structure by index - Write operation
void TopicPrivateUserDefinition::update(int index, const TopicPrivateUserStructure &updated_struct) {
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    if (index >= 0 && index < static_cast<int>(privateUserStructure.size())) {
        privateUserStructure[index] = updated_struct;
    } else {
        // Optionally handle the error, maybe throw an exception or log an error
    }
}

// Search for a structure by offset and return its index - Read operation
int TopicPrivateUserDefinition::searchByOffset(boost::multiprecision::uint128_t offset) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto it = std::find_if(privateUserStructure.begin(), privateUserStructure.end(),
                           [&offset](const TopicPrivateUserStructure &entry) {
                               return entry.getOffset() == offset;
                           });

    if (it != privateUserStructure.end()) {
        return std::distance(privateUserStructure.begin(), it);
    }
    return -1; // Not found
}

// Search for a structure by specific criteria and return it - Read operation
TopicPrivateUserStructure TopicPrivateUserDefinition::searchByCriteriaTypeReturn(const std::string &username,
                                                                                 const std::string &email) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto it = std::find_if(privateUserStructure.begin(), privateUserStructure.end(),
                           [&username, email](const TopicPrivateUserStructure &entry) {
                               return entry.getUsername() == username  ||
                                      entry.getEmail() == email;
                           });

    if (it != privateUserStructure.end()) {
        return *it;
    }

    return {};  // or throw an exception based on your error handling strategy
}

int TopicPrivateUserDefinition::searchByCriteria(const std::string &username,
                                                 const std::string &email) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    auto it = std::find_if(privateUserStructure.begin(), privateUserStructure.end(),
                           [&username, email ](const TopicPrivateUserStructure &entry) {
                               return (entry.getUsername() == username ||
                                       entry.getEmail() == email);
                           });

    if (it != privateUserStructure.end()) {
        return std::distance(privateUserStructure.begin(), it);
    }
    return -1; // Not found
}


// Print all TopicPrivateOffsetStructures in the privateOffsetStructure vector - Read operation
void TopicPrivateUserDefinition::printAll() {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    if (privateUserStructure.empty()) {
        std::cout << "No entries available." << std::endl;
    } else {
        for (const auto &structure: privateUserStructure) {
            printStruct(structure);
        }
    }
}

// Print the details of a single TopicPrivateOffsetStructure - Read operation
void TopicPrivateUserDefinition::printStruct(const TopicPrivateUserStructure &p_struct) {
    std::cout << "Offset: " << p_struct.getOffset() << ", "
              << "Timestamp: " << p_struct.getTimestamp() << ", "
              << "Username: " << p_struct.getUsername() << ", "
              << "Password: " << p_struct.getPassword() << ", "
              << "Email: " << p_struct.getEmail() << ", "
              << "Deleted: " << p_struct.isDeleted() << std::endl;
}

// Print the details of a single TopicPrivateOffsetStructure based on index - Read operation
void TopicPrivateUserDefinition::printStruct(int index) {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    if (index >= 0 && index < static_cast<int>(privateUserStructure.size())) {
        printStruct(privateUserStructure[index]);
    }
}

std::vector<TopicPrivateUserStructure> TopicPrivateUserDefinition::getAllUsers() {
    boost::shared_lock<boost::shared_mutex> lock(mutex_);
    return privateUserStructure;

}

