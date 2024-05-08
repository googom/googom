//
// Created by root on 5/6/24.
//

#ifndef GOOGOM_INITIALIZATION_MANAGER_H
#define GOOGOM_INITIALIZATION_MANAGER_H

#include "config_manager.h"
#include "topic_manager.h"

//THE GOAL OF THIS CLASS IS TO INITIALIZE ALL THE COMPONENTS
// IF SYSTEM TOPICS ARE NOT THERE, THEY WILL BE CREATED
// IF SYSTEM TABLES ARE THERE WILL DO THE MEMORY LOADING ONLY

class InitializationManager{

public:
    InitializationManager();

    void initialize(const std::string &configFile);
};

#endif //GOOGOM_INITIALIZATION_MANAGER_H
