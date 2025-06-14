#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_map>
#include <string>
#include <vector>

// Representa la ubicación de un bloque en un nodo disco
struct BlockLocation {
    size_t diskIndex;    // Índice del nodo disco en el RAID
    size_t stripeIndex;  // Índice del stripe
    bool isParity;       // Si es bloque de paridad
    size_t blockId;      // ID del bloque dentro del nodo
};

// Mapa de bloques de un documento
class BlockMap {
public:
    // Asocia un documento con sus bloques distribuidos
    void addDocument(const std::string& docName, const std::vector<std::vector<BlockLocation>>& blockLocations, size_t docSize);

    // Obtiene la ubicación de los bloques de un documento
    const std::vector<std::vector<BlockLocation>>* getBlockLocations(const std::string& docName) const;

    // Elimina un documento del mapa
    void removeDocument(const std::string& docName);

    // Obtiene el tamaño original del documento
    size_t getDocumentSize(const std::string& docName) const;

    // Devuelve todas las ubicaciones de bloques por documento
    std::unordered_map<std::string, std::vector<BlockLocation>> getAllLocations() const {
        std::unordered_map<std::string, std::vector<BlockLocation>> allLocations;
        for (const auto& pair : docMap) {
            std::vector<BlockLocation> flatLocations;
            for (const auto& stripe : pair.second.locations) {
                flatLocations.insert(flatLocations.end(), stripe.begin(), stripe.end());
            }
            allLocations[pair.first] = std::move(flatLocations);
        }
        return allLocations;
    }

private:
    struct DocInfo {
        std::vector<std::vector<BlockLocation>> locations; // [stripe][disk]
        size_t size;
    };
    std::unordered_map<std::string, DocInfo> docMap;
};
