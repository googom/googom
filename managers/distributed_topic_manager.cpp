//
// Created by root on 6/15/24.
//

#include "distributed_topic_manager.h"
#include "notification_manager.h"

extern seastar::sharded<NotificationManager> notificationManager;

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
        // Set the callback for when a message is stored
        topic->set_on_message_stored_callback([](const std::string &topic, const std::string &message) {
            std::cout << "Callback triggered for topic: " << topic << ", message: " << message << std::endl;
            notificationManager.invoke_on_all([topic, message](NotificationManager &nm) {
                return nm.notify_subscribers(topic, message).handle_exception([](std::exception_ptr eptr) {
                    try {
                        std::rethrow_exception(eptr);
                    } catch (const std::exception &e) {
                        std::cerr << "Error notifying subscribers: " << e.what() << std::endl;
                    }
                });
            });
        });
        topics_[key] = topic;
        return seastar::make_ready_future<std::shared_ptr<TopicPublicDefinition>>(topic);
    }
}

std::string DistributedTopicManager::generateKey(const std::string &topicName, int partition) const {
    return topicName + "_" + std::to_string(partition);
}