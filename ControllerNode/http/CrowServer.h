#pragma once
#include <crow_all.h>
#include <nlohmann/json.hpp>
#include "../manager/DocumentManager.h"

class CrowServer {
public:
    CrowServer(DocumentManager& docManager);
    void start(uint16_t port);
private:
    DocumentManager& docManager;
    crow::SimpleApp app;
    void setupRoutes();
}; 