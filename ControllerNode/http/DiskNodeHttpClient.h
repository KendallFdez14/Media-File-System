#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>

// Requiere Boost.Beast y Boost.Asio

class DiskNodeHttpClient {
public:
    // Almacena un bloque en el Disk Node
    // Devuelve true si fue exitoso, false en caso de error
    bool storeBlock(const std::string& ip, uint16_t port, uint64_t blockId, const std::vector<uint8_t>& data, bool isParity);

    // Recupera un bloque del Disk Node
    // Devuelve std::nullopt en caso de error
    std::optional<std::vector<uint8_t>> retrieveBlock(const std::string& ip, uint16_t port, uint64_t blockId);

private:
    // Métodos auxiliares para manejo de errores y reintentos
    bool handleError(const std::string& errorMsg);
}; 