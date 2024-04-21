#include "topic_debugging/topic_debugging.h"
#include "topic_public/topic_public_definition.h"
#include "topic_public/topic_public_message.h"
#include "topic_private/topic_offset/topic_private_offset_definition.h"

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

int main(int argc, char **argv) {
    seastar::app_template app;

    // Example usage
    const int bufferSize = 5;

    std::string text = "Hello World!";
    std::vector<uint8_t> binaryValue(text.begin(), text.end());

    // TopicDefinition topicDefinition("SampleTopic", 0, bufferSize,
    // "data.arrow");

    /*
     * std::shared_ptr<TopicPublicDefinition> sharedPtr
      = std::make_shared<TopicPublicDefinition>(
        "SampleTopic", 0, bufferSize, "data.googom");

    return app.run(argc, argv, [&]() -> seastar::future<int> {
        auto zero = co_await test(sharedPtr, binaryValue);

        co_return 0;
    });
     */

    TopicPrivateOffsetDefinition dataset;

    // Construct TopicPrivateOffsetStructure instances explicitly
    TopicPrivateOffsetStructure struct1(boost::multiprecision::uint128_t(100), 123456789, "topic1", "node1", 1,
                                        "type1");
    TopicPrivateOffsetStructure struct2(boost::multiprecision::uint128_t(200), 987654321, "topic2", "node2", 2,
                                        "type2");

    // Insert the constructed instances
    dataset.insert(struct1);
    dataset.insert(struct2);

    dataset.printAll();

    std::cout << "\n\n";

    // Update an existing instance
    TopicPrivateOffsetStructure updatedStruct(boost::multiprecision::uint128_t(100), 999999999, "updated_topic",
                                              "updated_node", 3, "updated_type");
    dataset.update(0, updatedStruct);

    // Print all instances
    dataset.printAll();

    // Search for a struct by offset
    int index = dataset.searchByOffset(boost::multiprecision::uint128_t(100));
    if (index != -1) {
        std::cout << "Struct found at index " << index << std::endl;
    } else {
        std::cout << "Struct not found" << std::endl;
    }

    std::cout << "\n\n";

    auto searchResult = dataset.searchByCriteriaTypeReturn("topic1", "node1", 3, "type1");

    dataset.printStruct(searchResult);

    auto se = dataset.searchByCriteriaTypeReturn("topic1", "node1", 3, "type1");

    std::cout << "type " << se.getType();

    return 0;
}
