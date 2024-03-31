#include "topic_io.h"

bool TopicIO::openFile(const std::string& filePath, std::fstream& file) {
    file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return false;
    }
    return true;
}

std::vector<TopicStructure> TopicIO::readRecordBatches(const std::string& filePath) {
    std::vector<TopicStructure> vector{};

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return vector;
    }

    // Read and deserialize TopicStructure objects
    while (true) {
        TopicStructure data;
        try {
            boost::archive::binary_iarchive ia(file);
            ia >> data;
        } catch (const boost::archive::archive_exception& e) {
            if (e.code == boost::archive::archive_exception::input_stream_error) {
                // Reached end of file
                break;
            } else {
                // Handle other archive exceptions
                std::cerr << "Error deserializing data: " << e.what() << std::endl;
                // Skip invalid data or handle the error as needed
                continue;
            }
        }

        // Process 'data' object
        vector.push_back(data);
    }

    file.close();

    return vector;
}


void TopicIO::writeToDisk(const std::string& filePath, const TopicStructure& data) {
    std::fstream file;
    file.open(filePath, std::ios::out | std::ios::app | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // Serialize and write TopicStructure object to file
    boost::archive::binary_oarchive oa(file);
    oa << data;

    file.close();
}
