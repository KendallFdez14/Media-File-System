#include "RAID5Manager.h"
#include <algorithm>
#include <stdexcept>

void RAID5Manager::stripeAndCalculateParity(const std::vector<uint8_t>& data, int numDisks, std::vector<std::vector<uint8_t>>& dataBlocks, std::vector<uint8_t>& parityBlock) {
    size_t totalBlocks = (data.size() + BLOCK_SIZE - 1) / BLOCK_SIZE;
    dataBlocks.clear();
    dataBlocks.resize(numDisks - 1); // N-1 bloques de datos, 1 de paridad
    for (auto& block : dataBlocks) block.resize(BLOCK_SIZE, 0);
    parityBlock.assign(BLOCK_SIZE, 0);

    // Llenar bloques de datos
    for (size_t i = 0; i < totalBlocks; ++i) {
        size_t offset = i * BLOCK_SIZE;
        size_t len = std::min(BLOCK_SIZE, data.size() - offset);
        int dataBlockIdx = i % (numDisks - 1);
        std::copy_n(data.begin() + offset, len, dataBlocks[dataBlockIdx].begin());
    }
    // Calcular paridad XOR
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        uint8_t p = 0;
        for (int d = 0; d < numDisks - 1; ++d) {
            p ^= dataBlocks[d][i];
        }
        parityBlock[i] = p;
    }
}

std::vector<uint8_t> RAID5Manager::recoverBlock(const std::vector<std::vector<uint8_t>>& dataBlocks, const std::vector<uint8_t>& parityBlock, int missingBlockIndex) {
    // Recupera un bloque perdido usando XOR de los otros bloques y la paridad
    std::vector<uint8_t> recovered(BLOCK_SIZE, 0);
    for (size_t i = 0; i < BLOCK_SIZE; ++i) {
        uint8_t val = parityBlock[i];
        for (int d = 0; d < dataBlocks.size(); ++d) {
            if (d != missingBlockIndex) {
                val ^= dataBlocks[d][i];
            }
        }
        recovered[i] = val;
    }
    return recovered;
} 