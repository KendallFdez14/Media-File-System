#include "BlockMap.h"

void BlockMap::registerBlock(const std::string& documentName, int diskNodeId, uint64_t blockIndex, bool isParity) {
    BlockLocation loc{diskNodeId, blockIndex, isParity};
    documentBlocks[documentName].blocks.push_back(loc);
    StoredBlockInfo info{blockIndex, documentName, isParity};
    diskNodes[diskNodeId].storedBlocks.push_back(info);
}

std::vector<BlockLocation> BlockMap::getBlockLocations(const std::string& documentName) const {
    auto it = documentBlocks.find(documentName);
    if (it != documentBlocks.end()) {
        return it->second.blocks;
    }
    return {};
} 