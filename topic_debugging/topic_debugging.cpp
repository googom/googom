//
// Created by root on 3/31/24.
//

#include "topic_debugging.h"

#include <fstream>

void TopicDebugging::printDiskData(std::string diskFilePath) {

    std::cout << "Data from disk:" << std::endl;
    for (const auto& item : topicIo.readRecordBatches(diskFilePath)) {
        std::cout << "Offset: " << item.getOffset() << std::endl;
        std::cout << "Timestamp: " << item.getTimestamp() << std::endl;
        std::cout << "Keys: " << item.getKeys() << std::endl;
        std::cout << "Headers: " << item.getHeaders() << std::endl;
        std::cout << "Flags: " << static_cast<int>(item.getFlags()) << std::endl;
        std::cout << "Value: ";
        const auto& valueByte = item.getValue();
        for (unsigned char byte : valueByte) {
            std::cout << static_cast<int>(byte) << " "; // Assuming you want to print byte values
        }
        std::cout << std::endl;
        std::cout << "Value: ";
        const auto& valueText = item.getValue();
        std::string text(valueText.begin(), valueText.end());
        std::cout << text << std::endl;
        std::cout << std::endl;
    }
}



void TopicDebugging::printBuffer(
  const std::vector<TopicStructure>& buffer,
  const std::string&
    name) { // Print data from the recentBuffer buffer (in-memory)
    std::cout << "In-memory data " << name << " :" << std::endl;
    for (const auto& data : buffer) {
        std::cout << "Offset: " << to_string(data.getOffset())
                  << ", Timestamp: " << data.getTimestamp()
                  << ", Headers: " << data.getHeaders() << ", Value: ";
        for (const auto& byte : data.getValue()) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(byte);
        }
        std::cout << std::endl;
    }

    std::cout << std::resetiosflags(std::ios::basefield);
}
TopicDebugging::TopicDebugging() {}
