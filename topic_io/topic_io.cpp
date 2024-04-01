#include "topic_io.h"

bool TopicIO::fileExists(const std::string& filePath) {
    std::fstream file;
    bool fileExists{false};
    try {
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "1. Error opening file: " << filePath << std::endl;
            fileExists = false;
        } else {
            fileExists = true;
        }
        file.close();
    } catch (std::exception exception) {
        fileExists = false;
    }
    return fileExists;
}


std::vector<TopicStructure> TopicIO::fillVector(const std::string& filePath) {
    vectorAllData.clear();

    // std::vector<TopicStructure> tempVector{};

    if (!fileExists(filePath)) {
        std::cerr << "2 Error opening file: " << filePath << std::endl;
        return vectorAllData;
    }

    std::ifstream file(filePath, std::ios::binary);

    // Read and deserialize TopicStructure objects
    while (true) {
        TopicStructure data;

        try {
            boost::archive::binary_iarchive ia(file);
            ia >> data;
        } catch (const boost::archive::archive_exception& e) {
            if (
              e.code == boost::archive::archive_exception::input_stream_error) {
                // Reached end of file
                break;
            } else {
                // Handle other archive exceptions
                std::cerr << "Error deserializing data: " << e.what()
                          << std::endl;
                // Skip invalid data or handle the error as needed
                continue;
            }
        }

        // Process 'data' object
        vectorAllData.push_back(data);
    }

    file.close();

    return vectorAllData;
}

void TopicIO::writeToDisk(
  const std::string& filePath, const TopicStructure& data) {
    if (!sharedFile.is_open()) openSharedFile(filePath);

    if (!sharedFile.is_open()) {
        std::cerr << "3 Error opening file: " << filePath << std::endl;
        return;
    }

    // Serialize and write TopicStructure object to file
    boost::archive::binary_oarchive oa(sharedFile);
    oa << data;
}

void TopicIO::openSharedFile(const std::string& filePath) {
    sharedFile.open(filePath, std::ios::out | std::ios::app | std::ios::binary);
}

void TopicIO::closeSharedFile() { sharedFile.close(); }

// TODO dump results to a temp file in case of Panic
TopicIO::~TopicIO() { closeSharedFile(); }

std::vector<TopicStructure>
TopicIO::readAllRecords(const std::string& filePath) {
   if(vectorAllData.empty()){
       fillVector(filePath);
   }
   return vectorAllData;
}

std::vector<TopicStructure>
TopicIO::readRecordsFromHead(const std::string& filePath, int fromHead) {
    std::vector<TopicStructure> vector{};
    vector.reserve(fromHead);

    if(vectorAllData.empty()){
        fillVector(filePath);
    }

    // Counter to keep track of how many elements have been processed
    int count = 0;
    // Iterator pointing to the beginning of the vector
    auto it = vectorAllData.begin();

    // Process elements from the head of the vector
    while (it != vectorAllData.end() && count < fromHead) {
        vector.push_back(*it); // Push the element to the result vector
        ++count;               // Increment the counter
        ++it;                  // Move the iterator to the next element
    }

    return vector;
}

std::vector<TopicStructure>
TopicIO::readRecordsFromTail(const std::string& filePath, int fromTail) {
    std::vector<TopicStructure> vector{};
    vector.reserve(fromTail);

    if(vectorAllData.empty()){
        fillVector(filePath);
    }

    // Counter to keep track of how many elements have been processed
    int count = 0;
    // Iterator pointing to the end of the vector
    auto it = vectorAllData.rbegin();

    // Process elements from the tail of the vector
    while (it != vectorAllData.rend() && count < fromTail) {
        vector.push_back(*it); // Push the element to the result vector
        ++count;               // Increment the counter
        ++it;                  // Move the iterator to the next element
    }

    return vector;
}
