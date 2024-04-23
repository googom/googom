//
// Created by root on 4/8/24.
//

#include "utils.h"
#include <iostream>
#include <chrono>

unsigned long long Utils::getCurrentMicroseconds() {
    // Get the current time point
    auto currentTime = std::chrono::system_clock::now();

    // Convert the time point to a duration since epoch
    auto durationSinceEpoch = currentTime.time_since_epoch();

    // Convert the duration to microseconds
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(durationSinceEpoch);

    // Convert the microseconds to a count and return
    return microseconds.count();
}
