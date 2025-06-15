#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

// Cliente HTTP para interactuar con los nodos de disco
class DiskNodeHttpClient {
public:
    // Escribe un bloque en un nodo de disco remoto
    bool writeBlock(const std::string& ip, uint16_t port, size_t blockId, const std::vector<uint8_t>& data);
    // Lee un bloque de un nodo de disco remoto
    std::optional<std::vector<uint8_t>> readBlock(const std::string& ip, uint16_t port, size_t blockId);
    // Verifica si el nodo de disco está disponible
    bool ping(const std::string& ip, uint16_t port);
};

