# Topic structure

## The schema of the topic:

Offset(Auto-increment boost int value)

Timestamp(Timestamp)

Keys(Json object)

Headers(Json object)

Flags(int value)

Value(unlimited string object)

## Main requirements for the topic
1. All the data should be persistent
2. We need to have buffer on both ends. The most recent inserted and the earliest instered data
3. We need to use multiprecision libraries from boost library.
4. Offset should be incremented on the insert time automatically
5. Topic should be searchable by its key
6. Consumer should be able to filter which record they needs


## Topic types
Append: add to the very end of the file.

Complete: update the record by its keys

## Custom events
Topic should produce custom events for downstream applications when something happened such as schema changed or deleted