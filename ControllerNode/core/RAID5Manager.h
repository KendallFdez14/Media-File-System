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
struct DiskNodeInfo {
    std::string ip;
    uint16_t port;
   
};

// Clase que gestiona la lógica de RAID 5
class RAID5Manager {
public:
    // Constructor: inicializa el número de discos y el tamaño de bloque
    RAID5Manager(size_t numDisks, size_t blockSize);

    // Divide los datos en bloques y calcula la paridad
    std::vector<std::vector<Block>> stripeData(const std::vector<uint8_t>& data);

    // Reconstruye los datos a partir de los bloques y la paridad
    std::vector<uint8_t> reconstructData(const std::vector<std::vector<Block>>& stripes, size_t originalSize);

    // Agrega un nodo disco al RAID
    void addDiskNode(const DiskNodeInfo& node);

    // Obtiene los nodos disco
    const std::vector<DiskNodeInfo>& getDiskNodes() const;

    // Devuelve la información del nodo de disco por índice
    const DiskNodeInfo& getDiskNode(size_t index) const {
        return diskNodes.at(index);
    }

    // Devuelve el tamaño de bloque
    size_t getBlockSize() const;
    // Devuelve el número de discos
    size_t getNumDisks() const;

private:
    size_t numDisks;
    size_t blockSize;
    std::vector<DiskNodeInfo> diskNodes;
};
