#include "topic_public/topic_definition.h"
#include "topic_public/topic_structure.h"

int main() {
    // Example usage
    const int bufferSize = 5;
    TopicDefinition storage("SampleTopic", 0, bufferSize, "data.arrow");

    TopicStructure sampleData = {
      "2024-03-29",
      "{}",
      "{}",
      0,
      {0xde, 0xad, 0xbe, 0xef} // Example binary data
    };

    // Simulate inserting multiple data entries
    for (int i = 0; i < 100; ++i) {
        storage.insert(sampleData);
    }

    std::cout << "Data insertion completed." << std::endl;

    storage.printAllData();

    return 0;
}
