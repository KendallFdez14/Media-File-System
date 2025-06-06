#pragma once
#include <vector>
#include <cstdint>

// RAID 5: N Disk Nodes, N-1 bloques de datos, 1 bloque de paridad rotatoria
// El tamaño de bloque y el número de nodos deben ser iguales en todos los Disk Nodes
constexpr size_t BLOCK_SIZE = 4096;
constexpr int NUM_DISK_NODES = 4;

class RAID5Manager {
public:
    // Divide los datos en bloques y calcula la paridad (XOR)
    // dataBlocks: N-1 bloques de datos, parityBlock: bloque de paridad
    void stripeAndCalculateParity(const std::vector<uint8_t>& data, int numDisks, std::vector<std::vector<uint8_t>>& dataBlocks, std::vector<uint8_t>& parityBlock);
    // Recupera un bloque perdido usando la paridad y los otros bloques
    std::vector<uint8_t> recoverBlock(const std::vector<std::vector<uint8_t>>& dataBlocks, const std::vector<uint8_t>& parityBlock, int missingBlockIndex);
}; 