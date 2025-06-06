#pragma once
#include <string>
#include <vector>
#include <cstdint>

enum class DiskNodeStatus {
    ACTIVE,
    INACTIVE,
    FAILED
};

struct StoredBlockInfo {
    uint64_t blockIndex;
    std::string documentName;
    bool isParity;
};

struct DiskNodeState {
    int id;
    std::string ip;
    uint16_t port;
    uint64_t totalCapacity;
    uint64_t availableSpace;
    DiskNodeStatus status;
    std::vector<StoredBlockInfo> storedBlocks;
}; 