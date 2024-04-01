#ifndef GOOGOM_TOPIC_IO_H
#define GOOGOM_TOPIC_IO_H

#include "../topic_public/topic_structure.h"

#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <fstream>

class TopicIO {
public:
    bool openFile(const std::string& filePath, std::fstream& file);
    std::vector<TopicStructure> readRecordBatches(const std::string& filePath);
    void writeToDisk(const std::string& filePath, const TopicStructure& data);
    //virtual ~TopicIO();
};
// TODO dump results to a temp file in case of Panic
// TopicIO::~TopicIO() {}
#endif // GOOGOM_TOPIC_IO_H
