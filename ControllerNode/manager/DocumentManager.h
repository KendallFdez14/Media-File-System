#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include "../core/BlockMap.h"
#include "../core/RAID5Manager.h"
#include "../http/DiskNodeHttpClient.h"

class DocumentManager {
public:
    DocumentManager(BlockMap& blockMap, RAID5Manager& raid5, DiskNodeHttpClient& diskClient);

    // Agrega un documento, lo divide en bloques y lo mapea
    void addDocument(const std::string& name, const std::vector<uint8_t>& data);

    // Elimina un documento
    void deleteDocument(const std::string& name);

    // Recupera los datos de un documento
    std::vector<uint8_t> getDocument(const std::string& name);

    // Consulta si existe un documento
    bool exists(const std::string& name) const;

private:
    BlockMap& blockMap;
    RAID5Manager& raid5;
    DiskNodeHttpClient& diskClient;
};
