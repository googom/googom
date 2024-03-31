#include "topic_debugging/topic_debugging.h"
#include "topic_public/topic_definition.h"
#include "topic_public/topic_structure.h"

int main() {
    // Example usage
    const int bufferSize = 5;

    std::string text = "Hello World!";
    std::vector<uint8_t> binaryValue(text.begin(), text.end());

    TopicDefinition topicDefinition("SampleTopic", 0, bufferSize, "data.arrow");

    TopicStructure topicStructure = {
      "2024-03-29",
      "{}",
      "{}",
      0,
      binaryValue // Example binary data
    };

    // Simulate inserting multiple data entries
    for (int i = 0; i < 100; ++i) {
        topicDefinition.insert(topicStructure);
    }

    std::cout << "Data insertion completed." << std::endl;

    TopicDebugging topicDebugging = TopicDebugging();
    topicDebugging.printBuffer(
      topicDefinition.getRecentBuffer(), "recentBuffer");
    topicDebugging.printDiskData("data.arrow");
    topicDebugging.printBuffer(
      topicDefinition.getOldestBuffer(), "oldestBuffer");

    return 0;
}
