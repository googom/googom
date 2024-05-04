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
class TopicPrivateOffsetStructure {
    boost::multiprecision::uint128_t offset;
    unsigned long long timestamp;
    std::string topic;
    std::string nodeId; //leader node
    uint8_t partition;
    //START and END should be different entries
    std::string type;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & offset;
        ar & timestamp;
        ar & topic;
        ar & nodeId;
        ar & partition;
        ar & type;
    }

public:
    TopicPrivateOffsetStructure() {}

    TopicPrivateOffsetStructure(
            const std::string &topic, const std::string &nodeId, uint8_t partition,
            const std::string &type) : topic(topic),
                                       nodeId(nodeId), partition(partition), type(type) {}

    TopicPrivateOffsetStructure(const boost::multiprecision::uint128_t &offset, unsigned long long int timestamp,
                                const std::string &topic, const std::string &nodeId, uint8_t partition,
                                const std::string &type) : offset(offset), timestamp(timestamp), topic(topic),
                                                           nodeId(nodeId), partition(partition), type(type) {}

    const boost::multiprecision::uint128_t &getOffset() const {
        return offset;
    }

    void setOffset(const boost::multiprecision::uint128_t &offset) {
        TopicPrivateOffsetStructure::offset = offset;
    }

    unsigned long long int getTimestamp() const {
        return timestamp;
    }

    void setTimestamp(unsigned long long int timestamp) {
        TopicPrivateOffsetStructure::timestamp = timestamp;
    }

    const std::string &getTopic() const {
        return topic;
    }

    void setTopic(const std::string &topic) {
        TopicPrivateOffsetStructure::topic = topic;
    }

    const std::string &getNodeId() const {
        return nodeId;
    }

    void setNodeId(const std::string &nodeId) {
        TopicPrivateOffsetStructure::nodeId = nodeId;
    }

    uint8_t getPartition() const {
        return partition;
    }

    void setPartition(uint8_t partition) {
        TopicPrivateOffsetStructure::partition = partition;
    }

    const std::string &getType() const {
        return type;
    }

    void setType(const std::string &type) {
        TopicPrivateOffsetStructure::type = type;
    }


};

#endif // GOOGOM_TOPIC_PRIVATE_OFFSET_STRUCTURE_H
