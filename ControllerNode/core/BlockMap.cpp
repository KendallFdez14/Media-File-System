#include "BlockMap.h"

void BlockMap::addDocument(const std::string& docName, const std::vector<std::vector<BlockLocation>>& blockLocations, size_t docSize) {
    docMap[docName] = DocInfo{blockLocations, docSize};
}

const std::vector<std::vector<BlockLocation>>* BlockMap::getBlockLocations(const std::string& docName) const {
    auto it = docMap.find(docName);
    if (it != docMap.end()) {
        return &it->second.locations;
    }
    return nullptr;
}

void BlockMap::removeDocument(const std::string& docName) {
    docMap.erase(docName);
}

size_t BlockMap::getDocumentSize(const std::string& docName) const {
    auto it = docMap.find(docName);
    if (it != docMap.end()) {
        return it->second.size;
    }
    return 0;
}
