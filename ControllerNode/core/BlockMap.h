#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "BlockMetadata.h"
#include "DiskNodeState.h"

struct BlockMap {
    // documentName -> BlockMetadata
    std::unordered_map<std::string, BlockMetadata> documentBlocks;
    // diskNodeId -> DiskNodeState
    std::unordered_map<int, DiskNodeState> diskNodes;

    // Métodos utilitarios
    void registerBlock(const std::string& documentName, int diskNodeId, uint64_t blockIndex, bool isParity);
    std::vector<BlockLocation> getBlockLocations(const std::string& documentName) const;
}; 