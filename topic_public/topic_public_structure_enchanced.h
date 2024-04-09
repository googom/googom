//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_PUBLIC_STRUCTURE_H
#define GOOGOM_TOPIC_PUBLIC_STRUCTURE_H

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <boost/multiprecision/cpp_int.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>


#include <iostream>
#include <utility>
#include "topic_public_structure.h"

// Adjusted to use a string for offset to work around Apache Arrow type
// limitations
class TopicPublicStructureEnchanced:public TopicPublicStructure {
    boost::multiprecision::uint128_t offset;
    unsigned long long timestamp;
    std::string keys;
    std::string headers;
    // THIS IS THE CUSTOM FLAG. IT WILL SHOW WHAT HAPPENED AND ALSO IN CASE OF
    // AN SYSTEM MESSAGE. IT WILL HAVE SYSTEM CODE: SCHEMA CHANGED, DELETED.
    // VALUE WILL BE EMPTY BASICALLY< THIS CUSTOM FLAG IS TO COMMUNICATE
    // DOWNSTREAM APPLICATIONS
    //  0 - NORMAL OPERATION
    //  1 - DATA CHANGED
    //  2 - SCHEMA CHANGED
    //  3 - TOPIC DELETED
    //  4 - ANNOUNCEMENT BY THE TOPIC OWNER -> IN THIS CASE, VALUE WILL HAVE THE
    //  INFORMATION OF THE ANNOUNCEMENT. IT COULD BE SOMETHING DECOMISSIONING
    std::int8_t flags;
    std::vector<uint8_t> value; // Representing binary data

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & offset;
        ar & timestamp;
        ar & keys;
        ar & headers;
        ar & flags;
        ar & value;
    }

public:
    TopicPublicStructureEnchanced() {}

    TopicPublicStructureEnchanced(
      unsigned long long timestamp,
      std::string keys,
      std::string headers,
      int8_t flags,
      const std::vector<uint8_t>& value)
      : timestamp(timestamp)
      , keys(std::move(keys))
      , headers(std::move(headers))
      , flags(flags)
      , value(value) {}

    const boost::multiprecision::uint128_t& getOffset() const { return offset; }
    void setOffset(const boost::multiprecision::uint128_t& offset) {
        TopicPublicStructureEnchanced::offset = offset;
    }
    const unsigned long long& getTimestamp() const { return timestamp; }
    void setTimestamp(const unsigned long long& timestamp) {
        TopicPublicStructureEnchanced::timestamp = timestamp;
    }
    const std::string& getKeys() const { return keys; }
    void setKeys(const std::string& keys) { TopicPublicStructureEnchanced::keys = keys; }
    const std::string& getHeaders() const { return headers; }
    void setHeaders(const std::string& headers) {
        TopicPublicStructureEnchanced::headers = headers;
    }
    int8_t getFlags() const { return flags; }
    void setFlags(int8_t flags) { TopicPublicStructureEnchanced::flags = flags; }
    const std::vector<uint8_t>& getValue() const { return value; }
    void setValue(const std::vector<uint8_t>& value) {
        TopicPublicStructureEnchanced::value = value;
    }
};

#endif // GOOGOM_TOPIC_PUBLIC_STRUCTURE_H
