#pragma once
#include <vector>
#include <cstdint>
#include <string>

// Estructura para representar un bloque de datos o paridad
struct Block {
    std::vector<uint8_t> data;
    bool isParity;
};

// Estructura para representar un nodo disco
struct DiskNode {
    std::string ip;
    uint16_t port;
    // Aquí podrías agregar más info, como estado, path, etc.
};

class RAID5Manager {
public:
    RAID5Manager(size_t numDisks, size_t blockSize);

    // Divide los datos en bloques y calcula la paridad
    std::vector<std::vector<Block>> stripeData(const std::vector<uint8_t>& data);

    // Reconstruye los datos a partir de los bloques y la paridad
    std::vector<uint8_t> reconstructData(const std::vector<std::vector<Block>>& stripes, size_t originalSize);

    // Agrega un nodo disco al RAID
    void addDiskNode(const DiskNode& node);

    // Obtiene los nodos disco
    const std::vector<DiskNode>& getDiskNodes() const;

    size_t getBlockSize() const;
    size_t getNumDisks() const;

private:
    size_t numDisks;
    size_t blockSize;
    std::vector<DiskNode> diskNodes;
};
