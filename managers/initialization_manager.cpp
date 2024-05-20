//
// Created by root on 5/6/24.
//

#include "initialization_manager.h"
#include "../topics/topic_private/topic_user/topic_private_user_structure.h"
#include "../topics/topic_private/topic_user/topic_private_user_definition.h"


InitializationManager::InitializationManager() = default;

void InitializationManager::initialize(const std::string &configFile) {
    auto *config = ConfigManager::getInstance();
    config->loadConfig(configFile);

    if (!TopicManager::checkTopicExists("__users")) {
        TopicManager::createTopic("__users");


        TopicPrivateUserDefinition::getInstance()->insert("admin","admin","admin@test.com");

    }

}
