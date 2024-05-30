//
// Created by root on 5/23/24.
//

#ifndef communication_utils
#define communication_utils

#include <iostream>
#include <string>
#include <sstream>
#include <map>

enum class MessageType {
    COMMAND,
    MESSAGE,
    SYSTEM,
    UNKNOWN
};

struct ParsedResult {
    MessageType type;
    std::map<std::string, std::string> keyValuePairs;
    std::string message;
};

MessageType getMessageType(std::string_view input) {
    if (input.starts_with("COMMAND:")) return MessageType::COMMAND;
    if (input.starts_with("MESSAGE:")) return MessageType::MESSAGE;
    if (input.starts_with("SYSTEM:")) return MessageType::SYSTEM;
    return MessageType::UNKNOWN;
}


// This function uses string_view to avoid copying strings
std::string_view trim(std::string_view sv) {
    size_t first = sv.find_first_not_of(" \n\r\t");
    size_t last = sv.find_last_not_of(" \n\r\t");
    if (first == std::string_view::npos || last == std::string_view::npos)
        return std::string_view();  // All whitespace
    return sv.substr(first, last - first + 1);
}

ParsedResult parseInput(std::string_view input) {
    ParsedResult result;
    if (input.empty()) return result;

    result.type = getMessageType(input);
    size_t typeEndPos = input.find(':');
    if (typeEndPos == std::string_view::npos) return result;

    size_t dataStart = typeEndPos + 1;
    size_t messageStart = input.find('|', dataStart);
    size_t dataEnd = (messageStart == std::string_view::npos) ? input.size() : messageStart;

    size_t start = dataStart;
    while (start < dataEnd) {
        size_t end = input.find(',', start);
        if (end == std::string_view::npos || end > dataEnd) end = dataEnd;
        size_t separator = input.find('=', start);
        if (separator != std::string_view::npos && separator < end) {
            std::string_view key = trim(input.substr(start, separator - start));
            std::string_view value = trim(input.substr(separator + 1, end - separator - 1));
            result.keyValuePairs.emplace(std::string(key), std::string(value));
        }
        start = end + 1;
    }

    if (messageStart != std::string_view::npos && messageStart + 1 < input.size()) {
        result.message = std::string(trim(input.substr(messageStart + 1)));
    }

    return result;
}

#endif // communication_utils