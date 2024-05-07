//
// Created by root on 5/6/24.
//

#include "initialization_manager.h"



InitializationManager::InitializationManager() = default;

void InitializationManager::initialize(const std::string &configFile){
    auto* config = ConfigManager::getInstance();
    config->loadConfig(configFile);
}
