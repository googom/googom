#ifndef GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H
#define GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H

#include "topic_private_user_structure.h"
#include "../topic_offset/topic_private_offset_definition.h"
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
#include <boost/thread/shared_mutex.hpp>


//TODO persistence is missing!!
class TopicPrivateUserDefinition {
    static boost::shared_mutex mutex_;
    static std::shared_ptr<TopicPrivateUserDefinition> instance_;
    std::vector<TopicPrivateUserStructure> privateUserStructure;

    // Private constructor to prevent external instantiation
    TopicPrivateUserDefinition() {}

public:
    // Deleted copy constructor and assignment operator to prevent copies
    TopicPrivateUserDefinition(const TopicPrivateUserDefinition&) = delete;
    TopicPrivateUserDefinition& operator=(const TopicPrivateUserDefinition&) = delete;

    static std::shared_ptr<TopicPrivateUserDefinition> getInstance();

    void insert(const std::string &username, const std::string &password,const std::string &email);
    void update(int index, const TopicPrivateUserStructure &updated_struct);
    int searchByOffset(boost::multiprecision::uint128_t offset);
    TopicPrivateUserStructure searchByCriteriaTypeReturn(const std::string &username,
                                                         const std::string &email);
    int searchByCriteria(const std::string &username,
                         const std::string &email);
    void printAll();
    void printStruct(const TopicPrivateUserStructure& p_struct);
    void printStruct(int index);
};

#endif // GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H
