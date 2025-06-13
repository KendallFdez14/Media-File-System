#include "DocumentManager.h"
#include <stdexcept>

DocumentManager::DocumentManager(BlockMap& blockMap, RAID5Manager& raid5, DiskNodeHttpClient& diskClient)
    : blockMap(blockMap), raid5(raid5), diskClient(diskClient) {}

void DocumentManager::addDocument(const std::string& name, const std::vector<uint8_t>& data) {
    auto stripes = raid5.stripeData(data);
    std::vector<std::vector<BlockLocation>> locations;
    const auto& diskNodes = raid5.getDiskNodes();
    size_t blockSize = raid5.getBlockSize();
    for (size_t s = 0; s < stripes.size(); ++s) {
        std::vector<BlockLocation> stripeLoc;
        for (size_t d = 0; d < stripes[s].size(); ++d) {
            // Enviar bloque al Disk Node correspondiente
            if (d < diskNodes.size()) {
                // Asegura que cada bloque tenga exactamente blockSize bytes
                std::vector<uint8_t> padded = stripes[s][d].data;
                padded.resize(blockSize, 0);
                diskClient.writeBlock(diskNodes[d].ip, diskNodes[d].port, s, padded);
            }
            stripeLoc.push_back(BlockLocation{d, s, stripes[s][d].isParity});
        }
        locations.push_back(stripeLoc);
    }
    blockMap.addDocument(name, locations, data.size());
}

void DocumentManager::deleteDocument(const std::string& name) {
    blockMap.removeDocument(name);
    // Aquí iría la lógica para eliminar los bloques en los Disk Nodes
}

std::vector<uint8_t> DocumentManager::getDocument(const std::string& name) {
    auto* locations = blockMap.getBlockLocations(name);
    if (!locations) throw std::runtime_error("Documento no encontrado");
    size_t docSize = blockMap.getDocumentSize(name);
    const auto& diskNodes = raid5.getDiskNodes();
    std::vector<std::vector<Block>> stripes;
    for (size_t s = 0; s < locations->size(); ++s) {
        std::vector<Block> stripe;
        int missingIdx = -1;
        // Leer todos los bloques del stripe
        for (size_t d = 0; d < (*locations)[s].size(); ++d) {
            if (d < diskNodes.size()) {
                auto blockData = diskClient.readBlock(diskNodes[d].ip, diskNodes[d].port, s);
                if (blockData) {
                    Block block;
                    block.data = *blockData;
                    block.isParity = (*locations)[s][d].isParity;
                    stripe.push_back(block);
                } else {
                    // Bloque faltante
                    Block block;
                    block.data.clear();
                    block.isParity = (*locations)[s][d].isParity;
                    stripe.push_back(block);
                    if (missingIdx == -1) missingIdx = d;
                    else throw std::runtime_error("Más de un bloque faltante en un stripe: no se puede recuperar con RAID 5");
                }
            }
        }
        // Si hay un bloque faltante, reconstruirlo
        if (missingIdx != -1) {
            std::vector<uint8_t> recovered(raid5.getBlockSize(), 0);
            for (size_t d = 0; d < stripe.size(); ++d) {
                if (d == missingIdx) continue;
                for (size_t b = 0; b < stripe[d].data.size(); ++b) {
                    recovered[b] ^= stripe[d].data[b];
                }
            }
            stripe[missingIdx].data = recovered;
        }
        stripes.push_back(stripe);
    }
    // Reconstruir datos usando RAID5Manager
    return raid5.reconstructData(stripes, docSize);
}

bool DocumentManager::exists(const std::string& name) const {
    return blockMap.getBlockLocations(name) != nullptr;
}
