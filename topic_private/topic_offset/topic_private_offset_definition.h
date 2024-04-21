#ifndef GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H
#define GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H

#include "topic_private_offset_structure.h"
#include <arrow/api.h>
#include <arrow/type_traits.h>
#include <arrow/util/logging.h>
#include <arrow/memory_pool.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <arrow/array.h>
#include <arrow/scalar.h>

class TopicPrivateOffsetDefinition {
    std::vector<TopicPrivateOffsetStructure> privateOffsetStructure{};

public:
    TopicPrivateOffsetDefinition();

    void insert(const TopicPrivateOffsetStructure &new_struct);

    void update(int index, const TopicPrivateOffsetStructure &updated_struct);

    int searchByOffset(boost::multiprecision::uint128_t offset);


    TopicPrivateOffsetStructure searchByCriteriaTypeReturn(const std::string &topic,
                                                           const std::string &nodeId,
                                                           uint8_t partition,
                                                           const std::string &type);

    int searchByCriteria(const std::string &topic,
                         const std::string &nodeId,
                         uint8_t partition,
                         const std::string &type);

    void printAll();  // Declaration for printing all structures

    // Function to print a single TopicPrivateOffsetStructure
    void printStruct(const TopicPrivateOffsetStructure& p_struct);

};

#endif //GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H
