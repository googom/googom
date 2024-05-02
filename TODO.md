Work with memory instead of file!!

Plan
1. Design topic structure
2. design topic sinks
3. design security
4. design TCP server


Obfuscation and encryption on topics(arrow files)

Default files location

Custom events such as schema changed or topic deleted on the topic with a key

## two topic types:

Append and update

Keys instead of key


## Data de-promotion

Mask and depromote the data to the lower envs. Connection should have been created

## buffer update

Both recent and oldest should be updated after insert and eater processes.

## topic blockers

Read should be blocked during eating operation. Writes should be moved to a new file


## problem statement

Internal topic for offset management. I don't want to load all the disk data to know statistics about the topic/partition. For Recent, i need to know what is the highest offset to get only last records instead of loading all the items into the heap

# Disk data dumpers
These processes should be different from memory operations.

# Leader and followers
Each of the partition should have a leader and follower nodes.

This information should be kept separately.


# Transport topic
I need a process to consume the transport topic. This is an internal topic to be shipped all the followers.

