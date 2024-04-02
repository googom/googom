# Public Topic structure

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

## Schema evaluation 
If set, the data has to be confirmed against the topic schema. This should be a flag in the topic creation time.

## Statistics
Topic statistics should be saved and exported

## Topic partitioning and replication
Each topic should be able to partitioned. Partitioning strategy should be dynamic.

Replication should be limited with available node count.


## Topic files 

### Topic files on the file system
These files should be partitioned by offset. For every 10.000 records, there should be a new file.
This value can be customizable.

### Topic files naming
topic-name-startOffset-endOffset

Parsing should check only last 2 dashes to make sure, we do not interfere with topic naming.

# Private Topics structure

These are internal topics. 

These topics will be used for governance and security.

These topics will start with __ (double dashes). So, the public topics cannot start with double dash

# Eater
A process should eat data from all the topics according to their defined retention periods.

# Configs

Configs should have information about:
1. Data storage location
2. Default retention period
3. Other nodes
4. Enabled comm channels

## Setup

Node should check if there is a config topic in place. if it is not there, then the application should read from config file.
Config file should be updated if there is a change in the config topic.