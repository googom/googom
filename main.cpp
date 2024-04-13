#include "topic_debugging/topic_debugging.h"
#include "topic_public/topic_public_definition.h"
#include "topic_public/topic_public_message.h"

#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>

#include <iostream>

seastar::future<int> test(
  std::shared_ptr<TopicPublicDefinition> topicPublicDefinition,
  std::vector<uint8_t> binaryValue) {
    // Simulate inserting multiple data entries
    for (int i = 0; i < 10; ++i) {
        co_await seastar::smp::submit_to(i % 8, [&topicPublicDefinition, binaryValue] {
            TopicPublicMessage topicPublicMessage = {
              "{}",
              "{}",
              binaryValue // Example binary data
            };

            topicPublicDefinition->insert(topicPublicMessage);
        });
    }

    std::cout << "Data insertion completed." << std::endl;

    TopicDebugging::printBuffer(
      topicPublicDefinition->getRecentBuffer(), "recentBuffer");
    TopicDebugging::printBuffer(
      topicPublicDefinition->getOldestBuffer(), "oldestBuffer");

    TopicDebugging::printDiskData("data.googom");

    co_return 0;
}

int main(int argc, char** argv) {
    seastar::app_template app;

    // Example usage
    const int bufferSize = 5;

    std::string text = "Hello World!";
    std::vector<uint8_t> binaryValue(text.begin(), text.end());

    // TopicDefinition topicDefinition("SampleTopic", 0, bufferSize,
    // "data.arrow");

    std::shared_ptr<TopicPublicDefinition> sharedPtr
      = std::make_shared<TopicPublicDefinition>(
        "SampleTopic", 0, bufferSize, "data.googom");

    return app.run(argc, argv, [&]() -> seastar::future<int> {
        auto zero = co_await test(sharedPtr, binaryValue);

        co_return 0;
    });
}
