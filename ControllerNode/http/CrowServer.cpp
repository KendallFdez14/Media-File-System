#include "CrowServer.h"
#include <cppcodec/base64_rfc4648.hpp>

using json = nlohmann::json;
using base64 = cppcodec::base64_rfc4648;

CrowServer::CrowServer(DocumentManager& docManager) : docManager(docManager) {}

void CrowServer::start(uint16_t port) {
    setupRoutes();
    app.port(port).multithreaded().run();
}

void CrowServer::setupRoutes() {
    CROW_ROUTE(app, "/documents").methods(crow::HTTPMethod::Post)([this](const crow::request& req){
        try {
            auto j = json::parse(req.body);
            if (!j.contains("name") || !j.contains("type") || !j.contains("file_data"))
                return crow::response(400, R"({"error":"Missing fields"})");
            std::string name = j["name"], type = j["type"], file_data = j["file_data"];
            auto data = base64::decode<std::vector<uint8_t>>(file_data);
            if (docManager.addDocument(name, type, data))
                return crow::response(201, R"({"status":"created"})");
            else
                return crow::response(500, R"({"error":"Failed to add document"})");
        } catch (const std::exception& e) {
            return crow::response(400, std::string(R"({"error":"Invalid JSON: ")") + e.what() + R"("})");
        }
    });

    CROW_ROUTE(app, "/documents/<string>").methods(crow::HTTPMethod::Delete)([this](const crow::request&, std::string name){
        if (docManager.deleteDocument(name))
            return crow::response(204);
        else
            return crow::response(404, R"({"error":"Document not found"})");
    });

    CROW_ROUTE(app, "/documents").methods(crow::HTTPMethod::Get)([this](const crow::request& req){
        auto url_params = crow::query_string(req.url_params);
        auto name = url_params.get("name");
        if (!name)
            return crow::response(400, R"({"error":"Missing name parameter"})");
        auto* meta = docManager.findDocument(name);
        if (meta) {
            json j = *meta;
            return crow::response(200, j.dump());
        } else {
            return crow::response(404, R"({"error":"Document not found"})");
        }
    });

    CROW_ROUTE(app, "/documents/<string>/download").methods(crow::HTTPMethod::Get)([this](const crow::request&, std::string name){
        auto data = docManager.downloadDocument(name);
        if (!data.empty()) {
            crow::response res(200);
            res.set_header("Content-Type", "application/octet-stream");
            res.body = std::string(data.begin(), data.end());
            return res;
        } else {
            return crow::response(404, R"({"error":"Document not found or failed to reconstruct"})");
        }
    });
} 