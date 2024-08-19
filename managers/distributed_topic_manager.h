//
// Created by root on 6/15/24.
//

#ifndef GOOGOM_DISTRIBUTED_TOPIC_MANAGER_H
#define GOOGOM_DISTRIBUTED_TOPIC_MANAGER_H

#include <seastar/core/sharded.hh>
#include <unordered_map>
#include <memory>
#include "../topics/topic_public/topic_public_definition.h"


class DistributedTopicManager {
public:
    seastar::future<> start();

    seastar::future<> stop();

    seastar::future<std::shared_ptr<TopicPublicDefinition>> getOrCreateTopicPublicDefinition(
            const std::string &topicName,
            int partition,
            int bufferSize,
            const std::string &diskFilePath
    );

    seastar::future<> enqueueMessage(const TopicPublicMessage& message);

    seastar::future<std::vector<TopicPublicMessage>> getUnconsumedMessages(const std::string& topic, const std::string& consumer_id);

    seastar::future<std::vector<TopicPublicMessage>> getQueueState() const;


private:
    std::string generateKey(const std::string &topicName, int partition) const;

    std::unordered_map<std::string, std::shared_ptr<TopicPublicDefinition>> topics_;

    static constexpr size_t max_total_messages = 1000;  // Maximum total number of messages
    static constexpr uint64_t max_offset_value = UINT64_MAX; // Max value for uint64_t, which is 18,446,744,073,709,551,615

    // Global queue storing all messages
    std::deque<TopicPublicMessage> global_message_queue;

    // Offsets for the messages in the queue
    std::deque<uint64_t> message_offsets;

    // Map from message offset to consumers that have consumed the message
    std::unordered_map<uint64_t, std::unordered_set<std::string>> message_consumers;

    seastar::future<uint64_t> generateOffset();
};


#endif //GOOGOM_DISTRIBUTED_TOPIC_MANAGER_H
