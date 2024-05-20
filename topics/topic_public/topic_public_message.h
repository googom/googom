//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_PUBLIC_MESSAGE_H
#define GOOGOM_TOPIC_PUBLIC_MESSAGE_H

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>
#include <boost/multiprecision/cpp_int.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>


#include <iostream>
#include <utility>

// Adjusted to use a string for offset to work around Apache Arrow type
// limitations
class TopicPublicMessage {
    std::string keys;
    std::string headers;
    std::vector<uint8_t> value; // Representing binary data

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & keys;
        ar & headers;
        ar & value;
    }

public:
    TopicPublicMessage() {}

    TopicPublicMessage(
      std::string keys,
      std::string headers,
      const std::vector<uint8_t>& value)
      : keys(std::move(keys))
      , headers(std::move(headers))
      , value(value) {}

    const std::string& getKeys() const { return keys; }
    void setKeys(const std::string& keys) { TopicPublicMessage::keys = keys; }
    const std::string& getHeaders() const { return headers; }
    void setHeaders(const std::string& headers) {
        TopicPublicMessage::headers = headers;
    }
    const std::vector<uint8_t>& getValue() const { return value; }
    void setValue(const std::vector<uint8_t>& value) {
        TopicPublicMessage::value = value;
    }
};

#endif // GOOGOM_TOPIC_PUBLIC_MESSAGE_H
