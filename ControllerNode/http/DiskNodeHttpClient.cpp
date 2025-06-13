#include "DiskNodeHttpClient.h"
#include <cpr/cpr.h>


bool DiskNodeHttpClient::writeBlock(const std::string& ip, uint16_t port, size_t blockId, const std::vector<uint8_t>& data) {
    std::string url = "http://" + ip + ":" + std::to_string(port) + "/write_block/" + std::to_string(blockId);
    auto resp = cpr::Post(cpr::Url{url}, cpr::Body{reinterpret_cast<const char*>(data.data()), data.size()});
    return resp.status_code == 200;
}

std::optional<std::vector<uint8_t>> DiskNodeHttpClient::readBlock(const std::string& ip, uint16_t port, size_t blockId) {
    std::string url = "http://" + ip + ":" + std::to_string(port) + "/read_block/" + std::to_string(blockId);
    auto resp = cpr::Get(cpr::Url{url});
    if (resp.status_code == 200) {
        return std::vector<uint8_t>(resp.text.begin(), resp.text.end());
    }
    return std::nullopt;
}

bool DiskNodeHttpClient::ping(const std::string& ip, uint16_t port) {
    std::string url = "http://" + ip + ":" + std::to_string(port) + "/ping";
    auto resp = cpr::Get(cpr::Url{url});
    return resp.status_code == 200;
}
