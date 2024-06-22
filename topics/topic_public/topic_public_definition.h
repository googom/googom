//
// Created by root on 3/31/24.
//

#ifndef GOOGOM_TOPIC_PUBLIC_DEFINITION_H
#define GOOGOM_TOPIC_PUBLIC_DEFINITION_H

#include "../topic_io/topic_io.h"
#include "topic_public_structure.h"
#include "topic_public_message.h"
#include "../../utilities/utils.h"

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/ipc/api.h>

#include <iostream>

class TopicPublicDefinition {
    const std::string topicName;
    const int partition;



    std::vector<TopicPublicStructure> recentBuffer{};
    std::vector<TopicPublicStructure> oldestBuffer{};

    // TODO should be generated on the fly!
    const std::string diskFilePath;
    const int bufferSize;

    boost::multiprecision::uint128_t getAutoIncrementedOffset();

    TopicIO topicIo = TopicIO();

    void initialLoadBufferFromDisk();

public:
    TopicPublicDefinition(
      std::string topicName,
      int partition,
      int bufferSize,
      std::string diskFilePath);

    boost::multiprecision::uint128_t insert(const TopicPublicMessage& data);


    // Callback function to notify about new messages
    std::function<void(const std::string&, const std::string&)> on_message_stored;

    // Set the callback for new messages
    void set_on_message_stored_callback(std::function<void(const std::string&, const std::string&)> callback) {
        on_message_stored = callback;
    }


    std::string topicFileNameGenerator(std::string topicName, int partition);

    const std::string& getTopicName() const;
    const int getPartition() const;
    const std::vector<TopicPublicStructure>& getRecentBuffer() const;
    void setRecentBuffer(const std::vector<TopicPublicStructure>& recentBuffer);
    const std::vector<TopicPublicStructure>& getOldestBuffer() const;
    void setOldestBuffer(const std::vector<TopicPublicStructure>& oldestBuffer);
    const std::string& getDiskFilePath() const;
    const int getBufferSize() const;

    TopicPublicStructure searchByOffset(const boost::multiprecision::uint128_t& offset);
    bool deleteByOffset(const boost::multiprecision::uint128_t& offset);
};

#endif // GOOGOM_TOPIC_PUBLIC_DEFINITION_H
