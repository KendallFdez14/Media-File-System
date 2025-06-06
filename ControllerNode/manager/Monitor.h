#pragma once
#include "../core/BlockMap.h"
#include "../core/RAID5Manager.h"
#include "../http/DiskNodeHttpClient.h"
#include <vector>
#include <string>

class Monitor {
public:
    Monitor(BlockMap& blockMap, RAID5Manager& raid5, DiskNodeHttpClient& diskClient);
    std::vector<DiskNodeState> getDiskNodeStates();
    std::vector<BlockMetadata> getAllDocumentMetadata();
    std::string getStatusReport();
private:
    BlockMap& blockMap;
    RAID5Manager& raid5;
    DiskNodeHttpClient& diskClient;
}; 