#pragma once
#include <string>
#include <vector>
#include "../core/BlockMap.h"
#include "../core/RAID5Manager.h"
#include "../http/DiskNodeHttpClient.h"

class DocumentManager {
public:
    DocumentManager(BlockMap& blockMap, RAID5Manager& raid5, DiskNodeHttpClient& diskClient);
    bool addDocument(const std::string& name, const std::string& type, const std::vector<uint8_t>& data);
    bool deleteDocument(const std::string& name);
    BlockMetadata* findDocument(const std::string& name);
    std::vector<uint8_t> downloadDocument(const std::string& name);
private:
    BlockMap& blockMap;
    RAID5Manager& raid5;
    DiskNodeHttpClient& diskClient;
}; 