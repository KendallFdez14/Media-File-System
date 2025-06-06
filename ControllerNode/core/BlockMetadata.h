#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>

struct BlockLocation {
    int diskNodeId;
    uint64_t blockIndex;
    bool isParity;
};

struct BlockMetadata {
    std::string documentName;
    std::string documentType;
    uint64_t documentSize;
    std::vector<BlockLocation> blocks;
};

void to_json(nlohmann::json& j, const BlockLocation& b);
void to_json(nlohmann::json& j, const BlockMetadata& m); 