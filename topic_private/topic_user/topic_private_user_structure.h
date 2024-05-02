//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_PRIVATE_OFFSET_STRUCTURE_H
#define GOOGOM_TOPIC_PRIVATE_OFFSET_STRUCTURE_H

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <boost/multiprecision/cpp_int.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <arrow/builder.h>


#include <iostream>
#include <utility>


//This should be in memory all the time!
class TopicPrivateUserStructure {
    boost::multiprecision::uint128_t offset;
    unsigned long long timestamp;
    std::string username;
    std::string password;
    std::string email;
    bool deleted;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & offset;
        ar & timestamp;
        ar & username;
        ar & password;
        ar & email;
        ar & deleted;
    }

public:
    TopicPrivateUserStructure() {}

    TopicPrivateUserStructure(
            const std::string &username, const std::string &password, const std::string &email,
            bool &deleted) : username(username),
                             password(password), email(email), deleted(deleted) {}

    TopicPrivateUserStructure(const boost::multiprecision::uint128_t &offset, unsigned long long int timestamp,
                              const std::string &username, const std::string &password, const std::string &email,
                              const bool &deleted) : offset(offset), timestamp(timestamp), username(username),
                                                     password(password), email(email), deleted(deleted) {}

    const boost::multiprecision::uint128_t &getOffset() const {
        return offset;
    }

    void setOffset(const boost::multiprecision::uint128_t &offset) {
        TopicPrivateUserStructure::offset = offset;
    }

    unsigned long long int getTimestamp() const {
        return timestamp;
    }

    void setTimestamp(unsigned long long int timestamp) {
        TopicPrivateUserStructure::timestamp = timestamp;
    }

    const std::string &getUsername() const {
        return username;
    }

    void setUsername(const std::string &username) {
        TopicPrivateUserStructure::username = username;
    }

    const std::string &getPassword() const {
        return password;
    }

    void setPassword(const std::string &password) {
        TopicPrivateUserStructure::password = password;
    }

    const std::string &getEmail() const {
        return email;
    }

    void setEmail(const std::string &email) {
        TopicPrivateUserStructure::email = email;
    }

    bool isDeleted() const {
        return deleted;
    }

    void setDeleted(bool deleted) {
        TopicPrivateUserStructure::deleted = deleted;
    }


};

#endif // GOOGOM_TOPIC_PRIVATE_OFFSET_STRUCTURE_H
