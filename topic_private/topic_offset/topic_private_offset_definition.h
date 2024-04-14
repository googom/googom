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


class MyStructDataset {
public:
    MyStructDataset() {
        schema_ = GetSchema();
    }

    void Insert(const TopicPrivateOffsetStructure& new_struct) {
        std::vector<TopicPrivateOffsetStructure> structs = {new_struct};
        auto new_batch = StructsToRecordBatch(structs);
        if (record_batch_) {
            record_batches_.push_back(new_batch);
        } else {
            record_batch_ = new_batch;
        }
    }

    void Update(int index, const TopicPrivateOffsetStructure& updated_struct) {
        if (index >= 0 && index < record_batch_->num_rows()) {
            UpdateStruct(index, updated_struct, record_batch_);
        }
    }

    int SearchByOffset(boost::multiprecision::uint128_t offset) {
        if (!record_batch_) {
            return -1;
        }
        return SearchStructByOffset(offset, record_batch_);
    }

    void PrintAll() {
        if (record_batch_) {
            for (int i = 0; i < record_batch_->num_rows(); ++i) {
                PrintStruct(i);
            }
        }
    }

private:
    std::shared_ptr<arrow::Schema> schema_;
    std::shared_ptr<arrow::RecordBatch> record_batch_;
    std::vector<std::shared_ptr<arrow::RecordBatch>> record_batches_;

    std::shared_ptr<arrow::Schema> GetSchema() {
        auto offset_field = field("offset_high", arrow::uint64());
        auto offset_field_low = field("offset_low", arrow::uint64());
        auto timestamp_field = field("timestamp", arrow::uint64());
        auto partition_field = field("partition", arrow::uint8());
        auto topic_field = field("topic", arrow::utf8());
        auto nodeId_field = field("nodeId", arrow::utf8());
        auto type_field = field("type", arrow::utf8());

        return arrow::schema({offset_field, offset_field_low, timestamp_field, partition_field, topic_field, nodeId_field, type_field});
    }

    std::shared_ptr<arrow::RecordBatch> StructsToRecordBatch(const std::vector<TopicPrivateOffsetStructure>& structs) {
        auto num_rows = static_cast<int64_t>(structs.size());

        // Create builders for each field
        arrow::UInt64Builder offset_high_builder;
        arrow::UInt64Builder offset_low_builder;
        arrow::UInt64Builder timestamp_builder;
        arrow::UInt8Builder partition_builder;
        arrow::StringBuilder topic_builder;
        arrow::StringBuilder nodeId_builder;
        arrow::StringBuilder type_builder;

        // Add data to builders
        for (const auto& s : structs) {
            offset_high_builder.Append(static_cast<uint64_t>(s.getOffset() >> 64));
            offset_low_builder.Append(static_cast<uint64_t>(s.getOffset() & 0xFFFFFFFFFFFFFFFF));
            timestamp_builder.Append(s.getTimestamp());
            partition_builder.Append(s.getPartition());
            topic_builder.Append(s.getTopic());
            nodeId_builder.Append(s.getNodeId());
            type_builder.Append(s.getType());
        }

        // Finalize the builders
        std::shared_ptr<arrow::Array> offset_high_array, offset_low_array, timestamp_array, partition_array, topic_array, nodeId_array, type_array;
        offset_high_builder.Finish(&offset_high_array);
        offset_low_builder.Finish(&offset_low_array);
        timestamp_builder.Finish(&timestamp_array);
        partition_builder.Finish(&partition_array);
        topic_builder.Finish(&topic_array);
        nodeId_builder.Finish(&nodeId_array);
        type_builder.Finish(&type_array);

        // Create a record batch
        return arrow::RecordBatch::Make(schema_, num_rows, {offset_high_array, offset_low_array, timestamp_array, partition_array, topic_array, nodeId_array, type_array});
    }

    void UpdateStruct(int index, const TopicPrivateOffsetStructure& updated_struct, std::shared_ptr<arrow::RecordBatch>& record_batch) {
        auto offset_high_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch->column(0));
        auto offset_low_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch->column(1));
        auto timestamp_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch->column(2));
        auto partition_array = std::static_pointer_cast<arrow::UInt8Array>(record_batch->column(3));
        auto topic_array = std::static_pointer_cast<arrow::StringArray>(record_batch->column(4));
        auto nodeId_array = std::static_pointer_cast<arrow::StringArray>(record_batch->column(5));
        auto type_array = std::static_pointer_cast<arrow::StringArray>(record_batch->column(6));

        // Create builders for updating arrays
        arrow::UInt64Builder offset_high_builder;
        arrow::UInt64Builder offset_low_builder;
        arrow::UInt64Builder timestamp_builder;
        arrow::UInt8Builder partition_builder;
        arrow::StringBuilder topic_builder;
        arrow::StringBuilder nodeId_builder;
        arrow::StringBuilder type_builder;

        // Append existing values to builders
        for (int i = 0; i < record_batch->num_rows(); ++i) {
            offset_high_builder.Append(offset_high_array->Value(i));
            offset_low_builder.Append(offset_low_array->Value(i));
            timestamp_builder.Append(timestamp_array->Value(i));
            partition_builder.Append(partition_array->Value(i));
            topic_builder.Append(topic_array->GetString(i));
            nodeId_builder.Append(nodeId_array->GetString(i));
            type_builder.Append(type_array->GetString(i));
        }

        // Update value at the specified index
        offset_high_builder[index] = static_cast<uint64_t>(updated_struct.getOffset() >> 64);
        offset_low_builder[index] = static_cast<uint64_t>(updated_struct.getOffset() & 0xFFFFFFFFFFFFFFFF);
        timestamp_builder[index] = updated_struct.getTimestamp();
        partition_builder[index] = updated_struct.getPartition();
        topic_builder.Append(updated_struct.getTopic());
        nodeId_builder.Append(updated_struct.getNodeId());
        type_builder.Append(updated_struct.getType());

        // Finish building the updated arrays
        std::shared_ptr<arrow::Array> updated_offset_high_array, updated_offset_low_array, updated_timestamp_array,
                updated_partition_array, updated_topic_array, updated_nodeId_array, updated_type_array;
        offset_high_builder.Finish(&updated_offset_high_array);
        offset_low_builder.Finish(&updated_offset_low_array);
        timestamp_builder.Finish(&updated_timestamp_array);
        partition_builder.Finish(&updated_partition_array);
        topic_builder.Finish(&updated_topic_array);
        nodeId_builder.Finish(&updated_nodeId_array);
        type_builder.Finish(&updated_type_array);

        // Replace old arrays with updated arrays
        record_batch = arrow::RecordBatch::Make(record_batch->schema(),
                                                record_batch->num_rows(),
                                                {updated_offset_high_array, updated_offset_low_array, updated_timestamp_array,
                                                 updated_partition_array, updated_topic_array, updated_nodeId_array, updated_type_array});
    }





    int SearchStructByOffset(boost::multiprecision::uint128_t offset, const std::shared_ptr<arrow::RecordBatch>& record_batch) {
        auto offset_high_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch->column(0));
        auto offset_low_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch->column(1));

        // Linear search for offset value
        for (int i = 0; i < record_batch->num_rows(); ++i) {
            uint64_t high = offset_high_array->Value(i);
            uint64_t low = offset_low_array->Value(i);
            boost::multiprecision::uint128_t current_offset = (static_cast<boost::multiprecision::uint128_t>(high) << 64) + low;
            if (current_offset == offset) {
                return i; // Return index if found
            }
        }

        return -1; // Return -1 if not found
    }

    void PrintStruct(int index) {
        auto offset_high_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch_->column(0));
        auto offset_low_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch_->column(1));
        auto timestamp_array = std::static_pointer_cast<arrow::UInt64Array>(record_batch_->column(2));
        auto partition_array = std::static_pointer_cast<arrow::UInt8Array>(record_batch_->column(3));
        auto topic_array = std::static_pointer_cast<arrow::StringArray>(record_batch_->column(4));
        auto nodeId_array = std::static_pointer_cast<arrow::StringArray>(record_batch_->column(5));
        auto type_array = std::static_pointer_cast<arrow::StringArray>(record_batch_->column(6));

        uint64_t high = offset_high_array->Value(index);
        uint64_t low = offset_low_array->Value(index);
        boost::multiprecision::uint128_t offset = (static_cast<boost::multiprecision::uint128_t>(high) << 64) + low;

        std::cout << "Index: " << index << std::endl;
        std::cout << "Offset: " << offset << std::endl;
        std::cout << "Timestamp: " << timestamp_array->Value(index) << std::endl;
        std::cout << "Partition: " << static_cast<int>(partition_array->Value(index)) << std::endl;
        std::cout << "Topic: " << topic_array->GetString(index) << std::endl;
        std::cout << "NodeId: " << nodeId_array->GetString(index) << std::endl;
        std::cout << "Type: " << type_array->GetString(index) << std::endl;
    }
};


#endif //GOOGOM_TOPIC_PRIVATE_OFFSET_DEFINITION_H
