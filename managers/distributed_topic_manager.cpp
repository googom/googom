//
// Created by root on 6/15/24.
//

#include "distributed_topic_manager.h"

seastar::future<> DistributedTopicManager::start() {
    return seastar::make_ready_future<>();
}

seastar::future<> DistributedTopicManager::stop() {
    return seastar::make_ready_future<>();
}

seastar::future<std::shared_ptr<TopicPublicDefinition>> DistributedTopicManager::getOrCreateTopicPublicDefinition(
        const std::string &topicName,
        int partition,
        int bufferSize,
        const std::string &diskFilePath
) {
    std::string key = generateKey(topicName, partition);


    auto it = topics_.find(key);
    if (it != topics_.end()) {
        return seastar::make_ready_future<std::shared_ptr<TopicPublicDefinition>>(it->second);
    } else {
        auto topic = std::make_shared<TopicPublicDefinition>(topicName, partition, bufferSize, diskFilePath);
        topics_[key] = topic;
        return seastar::make_ready_future<std::shared_ptr<TopicPublicDefinition>>(topic);
    }
}

std::string DistributedTopicManager::generateKey(const std::string &topicName, int partition) const {
    return topicName + "_" + std::to_string(partition);
}


// Enqueue a message with a global limit
seastar::future<> DistributedTopicManager::enqueueMessage(const TopicPublicMessage& message) {
    if (global_message_queue.size() >= max_total_messages) {
        auto oldest_message_offset = message_offsets.front();
        message_consumers.erase(oldest_message_offset);
        global_message_queue.pop_front();  // Remove the oldest message
    }


    // Handle the offset generation asynchronously
    return generateOffset().then([&message, this](uint64_t offset) {
        global_message_queue.push_back(message);
        message_offsets.push_back(offset);
        message_consumers[offset] = std::unordered_set<std::string>{};
        return seastar::make_ready_future<>();
    });
}

// Retrieve unconsumed messages by consumer and topic
seastar::future<std::vector<TopicPublicMessage>> DistributedTopicManager::getUnconsumedMessages(const std::string& topic, const std::string& consumer_id) {
    std::vector<TopicPublicMessage> messages;

    for (size_t i = 0; i < global_message_queue.size(); ++i) {
        const auto& message = global_message_queue[i];
        const auto& offset = message_offsets[i];

        if (message.getTopic() == topic && message_consumers[offset].find(consumer_id) == message_consumers[offset].end()) {
            messages.push_back(message);
            message_consumers[offset].insert(consumer_id);
        }
    }

    return seastar::make_ready_future<std::vector<TopicPublicMessage>>(std::move(messages));
}

// Check what is left in the queue
seastar::future<std::vector<TopicPublicMessage>> DistributedTopicManager::getQueueState() const {
    std::vector<TopicPublicMessage> queue_state(global_message_queue.begin(), global_message_queue.end());
    return seastar::make_ready_future<std::vector<TopicPublicMessage>>(std::move(queue_state));
}

// Generate a unique offset for each message with wrapping logic
seastar::future<uint64_t> DistributedTopicManager::generateOffset() {
    static uint64_t current_offset = 0;

    current_offset++;

    // Wrap around if we reach the maximum value
    if (current_offset == 0 || current_offset >= max_offset_value) {
        current_offset = 1; // Start back at 1 to avoid conflicts with 0 (assuming 0 could represent an invalid offset)
    }

    // Ensure the new offset does not conflict with existing active offsets
    while (std::find(message_offsets.begin(), message_offsets.end(), current_offset) != message_offsets.end()) {
        current_offset++;
        if (current_offset == 0 || current_offset >= max_offset_value) {
            current_offset = 1; // Wrap around again if needed
        }
    }

    return seastar::make_ready_future<uint64_t>(current_offset);
}
