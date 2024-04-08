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

namespace mp = boost::multiprecision;


//This should be in memory all the time!
class TopicPrivateOffsetStructure {
    mp::uint128_t offset;
    unsigned long long timestamp;
    std::string topic;
    uint8_t partition;
    //START and END should be different entries
    std::string type;

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & offset;
        ar & timestamp;
        ar & topic;
        ar & partition;
        ar & type;
    }

public:
    TopicPrivateOffsetStructure() {}

    TopicPrivateOffsetStructure(
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


};

#endif // GOOGOM_TOPIC_PUBLIC_STRUCTURE_H
