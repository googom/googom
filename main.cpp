#include "topic_debugging/topic_debugging.h"
#include "topic_public/topic_definition.h"
#include "topic_public/topic_structure.h"

#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>

seastar::future<int>
test(TopicDefinition& topicDefinition, std::vector<uint8_t> binaryValue) {
    TopicStructure topicStructure = {
      "2024-03-29",
      "{}",
      "{}",
      0,
      binaryValue // Example binary data
    };

    // Simulate inserting multiple data entries
    for (int i = 0; i < 10000; ++i) {
        co_await seastar::smp::submit_to(i%8, [&topicDefinition, &topicStructure] {
            topicDefinition.insert(topicStructure);
        });
    }

    std::cout << "Data insertion completed." << std::endl;

    TopicDebugging::printBuffer(
      topicDefinition.getRecentBuffer(), "recentBuffer");
    TopicDebugging::printBuffer(
      topicDefinition.getOldestBuffer(), "oldestBuffer");

    TopicDebugging::printDiskData("data.arrow");

    co_return 0;
}

int main(int argc, char** argv) {
    seastar::app_template app;

    // Example usage
    const int bufferSize = 5;

    std::string text = "Hello World!";
    std::vector<uint8_t> binaryValue(text.begin(), text.end());

    TopicDefinition topicDefinition("SampleTopic", 0, bufferSize, "data.arrow");

    return app.run(argc, argv, [&]() -> seastar::future<int> {
        co_await test(topicDefinition, binaryValue);

        co_return 0;
    });
}
