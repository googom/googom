#include "public_topic/topic_definition.h"
#include "public_topic/topic_structure.h"

int main() {
    // Example usage
    const int bufferSize = 5;
    topic_definition storage("SampleTopic", 0, bufferSize, "data.arrow");

    topic_structure sampleData = {
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
