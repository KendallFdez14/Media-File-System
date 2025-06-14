#include "RAID5Manager.h"
#include <stdexcept>
#include <algorithm>

RAID5Manager::RAID5Manager(size_t numDisks, size_t blockSize)
    : numDisks(numDisks), blockSize(blockSize) {}

void RAID5Manager::addDiskNode(const DiskNodeInfo& node) {
    diskNodes.push_back(node);
}

const std::vector<DiskNodeInfo>& RAID5Manager::getDiskNodes() const {
    return diskNodes;
}

size_t RAID5Manager::getBlockSize() const {
    return blockSize;
}

size_t RAID5Manager::getNumDisks() const {
    return numDisks;
}

// Divide los datos en stripes y calcula la paridad para cada stripe
std::vector<std::vector<Block>> RAID5Manager::stripeData(const std::vector<uint8_t>& data) {
    if (numDisks < 3) throw std::runtime_error("RAID 5 requiere al menos 3 discos");
    std::vector<std::vector<Block>> stripes;
    size_t dataDisks = numDisks - 1;
    size_t totalBlocks = (data.size() + blockSize - 1) / blockSize;
    size_t totalStripes = (totalBlocks + dataDisks - 1) / dataDisks;

    for (size_t s = 0; s < totalStripes; ++s) {
        std::vector<Block> stripe(numDisks);
        // Llenar bloques de datos
        for (size_t d = 0; d < dataDisks; ++d) {
            size_t blockIdx = s * dataDisks + d;
            size_t offset = blockIdx * blockSize;
            if (offset < data.size()) {
                size_t len = std::min(blockSize, data.size() - offset);
                stripe[d].data.assign(data.begin() + offset, data.begin() + offset + len);
                stripe[d].isParity = false;
            } else {
                stripe[d].data.clear();
                stripe[d].isParity = false;
            }
        }
        // Calcular paridad XOR
        std::vector<uint8_t> parity(blockSize, 0);
        for (size_t b = 0; b < blockSize; ++b) {
            for (size_t d = 0; d < dataDisks; ++d) {
                if (b < stripe[d].data.size())
                    parity[b] ^= stripe[d].data[b];
            }
        }
        stripe[dataDisks].data = parity;
        stripe[dataDisks].isParity = true;
        stripes.push_back(stripe);
    }
    return stripes;
}

// Reconstruye los datos a partir de los stripes (ignorando fallos por simplicidad)
std::vector<uint8_t> RAID5Manager::reconstructData(const std::vector<std::vector<Block>>& stripes, size_t originalSize) {
    std::vector<uint8_t> data;
    for (const auto& stripe : stripes) {
        for (size_t d = 0; d < numDisks - 1; ++d) {
            data.insert(data.end(), stripe[d].data.begin(), stripe[d].data.end());
            if (data.size() >= originalSize) break;
        }
        if (data.size() >= originalSize) break;
    }
    data.resize(originalSize);
    return data;
}
