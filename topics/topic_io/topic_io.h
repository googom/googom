#ifndef GOOGOM_TOPIC_IO_H
#define GOOGOM_TOPIC_IO_H

#include "../topic_public/topic_public_structure.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <fstream>

class TopicIO {
    std::vector<TopicPublicStructure> vectorAllData{};
    std::fstream sharedFile;

    void openSharedFile(const std::string& filePath);
    std::vector<TopicPublicStructure> fillVector(const std::string& filePath);

public:
    bool fileExists(const std::string& filePath);
    std::vector<TopicPublicStructure> readAllRecords(const std::string& filePath);
    std::vector<TopicPublicStructure> readRecordsFromHead(const std::string& filePath, int fromHead);
    std::vector<TopicPublicStructure> readRecordsFromTail(const std::string& filePath, int fromTail);
    void writeToDisk(const std::string& filePath, const TopicPublicStructure& data);
    void closeSharedFile();
    virtual ~TopicIO();

    TopicPublicStructure searchInArrowFiles(const std::string& filePath, boost::multiprecision::uint128_t offset);
    bool deleteFromArrowFiles(const std::string& filePath, boost::multiprecision::uint128_t offset);
};

#endif // GOOGOM_TOPIC_IO_H
