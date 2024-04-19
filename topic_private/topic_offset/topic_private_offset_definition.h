//
// Created by root on 4/13/24.
//

#ifndef GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H
#define GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H

#include "topic_private_offset_structure.h"
#include <arrow/api.h>
#include <arrow/type_traits.h>
#include <arrow/util/logging.h>
#include <arrow/memory_pool.h>
#include <boost/multiprecision/cpp_int.hpp>


class TopicPrivateOffsetDefinition {
public:
    TopicPrivateOffsetDefinition();

    void Insert(const TopicPrivateOffsetStructure& new_struct);

    void Update(int index, const TopicPrivateOffsetStructure& updated_struct);

    int SearchByOffset(boost::multiprecision::uint128_t offset);

    void PrintAll();

private:
    std::shared_ptr<arrow::Schema> schema_;
    std::shared_ptr<arrow::RecordBatch> record_batch_;
    std::vector<std::shared_ptr<arrow::RecordBatch>> record_batches_;

    std::shared_ptr<arrow::Schema> GetSchema();

    std::shared_ptr<arrow::RecordBatch> StructsToRecordBatch(const std::vector<TopicPrivateOffsetStructure>& structs);

    void UpdateStruct(int index, const TopicPrivateOffsetStructure& updated_struct, std::shared_ptr<arrow::RecordBatch>& record_batch);


    int SearchStructByOffset(boost::multiprecision::uint128_t offset, const std::shared_ptr<arrow::RecordBatch>& record_batch);

    void PrintStruct(int index);
};


#endif //GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H
