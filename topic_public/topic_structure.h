//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_STRUCTURE_H
#define GOOGOM_TOPIC_STRUCTURE_H


#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <boost/multiprecision/cpp_int.hpp>

#include <iostream>
#include <utility>

namespace mp = boost::multiprecision;

// Adjusted to use a string for offset to work around Apache Arrow type
// limitations
class TopicStructure {
    mp::uint128_t offset;
    std::string timestamp;
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

public:
    TopicStructure() {}

    TopicStructure(
      std::string timestamp,
      std::string keys,
      std::string headers,
      int8_t flags,
      const std::vector<uint8_t>& value)
      : timestamp(std::move(timestamp))
      , keys(std::move(keys))
      , headers(std::move(headers))
      , flags(flags)
      , value(value) {}

    const boost::multiprecision::uint128_t& getOffset() const { return offset; }
    void setOffset(const boost::multiprecision::uint128_t& offset) {
        TopicStructure::offset = offset;
    }
    const std::string& getTimestamp() const { return timestamp; }
    void setTimestamp(const std::string& timestamp) {
        TopicStructure::timestamp = timestamp;
    }
    const std::string& getKeys() const { return keys; }
    void setKeys(const std::string& keys) { TopicStructure::keys = keys; }
    const std::string& getHeaders() const { return headers; }
    void setHeaders(const std::string& headers) {
        TopicStructure::headers = headers;
    }
    int8_t getFlags() const { return flags; }
    void setFlags(int8_t flags) { TopicStructure::flags = flags; }
    const std::vector<uint8_t>& getValue() const { return value; }
    void setValue(const std::vector<uint8_t>& value) {
        TopicStructure::value = value;
    }
};

#endif // GOOGOM_TOPIC_STRUCTURE_H
