#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

class DiskNodeHttpClient {
public:
    bool writeBlock(const std::string& ip, uint16_t port, size_t blockId, const std::vector<uint8_t>& data);
    std::optional<std::vector<uint8_t>> readBlock(const std::string& ip, uint16_t port, size_t blockId);
    bool ping(const std::string& ip, uint16_t port);
};

