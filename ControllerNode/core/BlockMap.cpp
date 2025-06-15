#include "BlockMap.h"

// Agrega un documento al mapa de bloques
void BlockMap::addDocument(const std::string& docName, const std::vector<std::vector<BlockLocation>>& blockLocations, size_t docSize) {
    docMap[docName] = DocInfo{blockLocations, docSize};
}

// Obtiene las ubicaciones de los bloques de un documento
const std::vector<std::vector<BlockLocation>>* BlockMap::getBlockLocations(const std::string& docName) const {
    auto it = docMap.find(docName);
    if (it != docMap.end()) {
        return &it->second.locations;
    }
    return nullptr;
}

// Elimina un documento del mapa
void BlockMap::removeDocument(const std::string& docName) {
    docMap.erase(docName);
}

// Obtiene el tamaño original del documento
size_t BlockMap::getDocumentSize(const std::string& docName) const {
    auto it = docMap.find(docName);
    if (it != docMap.end()) {
        return it->second.size;
    }
    return 0;
}
