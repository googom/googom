#ifndef GOOGOM_TOPIC_IO_H
#define GOOGOM_TOPIC_IO_H

#include "../topic_public/topic_structure.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <fstream>

class TopicIO {
    std::vector<TopicStructure> vectorAllData{};
    std::fstream sharedFile;

    void openSharedFile(const std::string& filePath);
    std::vector<TopicStructure> fillVector(const std::string& filePath);

public:
    bool fileExists(const std::string& filePath);
    std::vector<TopicStructure> readAllRecords(const std::string& filePath);
    std::vector<TopicStructure> readRecordsFromHead(const std::string& filePath, int fromHead);
    std::vector<TopicStructure> readRecordsFromTail(const std::string& filePath, int fromTail);
    void writeToDisk(const std::string& filePath, const TopicStructure& data);
    void closeSharedFile();
    virtual ~TopicIO();
};

#endif // GOOGOM_TOPIC_IO_H
