#include <crow.h>
#include "core/BlockMap.h"
#include "core/RAID5Manager.h"
#include "manager/DocumentManager.h"
#include "http/DiskNodeHttpClient.h"
#include <nlohmann/json.hpp>
#include <fstream>

int main(int argc, char* argv[]) {
    crow::SimpleApp app;
    BlockMap blockMap;
    RAID5Manager raid5(4, 4096);
    raid5.addDiskNode({"127.0.0.1", 8000});
    raid5.addDiskNode({"127.0.0.1", 8001});
    raid5.addDiskNode({"127.0.0.1", 8002});
    raid5.addDiskNode({"127.0.0.1", 8003});
    DiskNodeHttpClient diskClient;
    DocumentManager docManager(blockMap, raid5, diskClient);

    // Endpoint: Agregar documento (POST /add)
    CROW_ROUTE(app, "/add").methods("POST"_method)([&](const crow::request& req) {
        auto name = req.url_params.get("name");
        if (!name) return crow::response(400, "Falta el parámetro 'name'");
        std::vector<uint8_t> data(req.body.begin(), req.body.end());
        // Log: imprime los primeros bytes recibidos
        std::cout << "[ADD] Primeros bytes recibidos: ";
        for (size_t i = 0; i < std::min<size_t>(data.size(), 8); ++i) std::cout << std::hex << (int)data[i] << " ";
        std::cout << std::dec << std::endl;
        try {
            docManager.addDocument(name, data);
            return crow::response(200, "Documento agregado");
        } catch (const std::exception& ex) {
            return crow::response(500, ex.what());
        }
    });

    // Endpoint: Eliminar documento (DELETE /delete?name=...)
    CROW_ROUTE(app, "/delete").methods("DELETE"_method)([&](const crow::request& req) {
        auto name = req.url_params.get("name");
        if (!name) return crow::response(400, "Falta el parámetro 'name'");
        try {
            docManager.deleteDocument(name);
            return crow::response(200, "Documento eliminado");
        } catch (const std::exception& ex) {
            return crow::response(500, ex.what());
        }
    });

    // Endpoint: Buscar documento (GET /exists?name=...)
    CROW_ROUTE(app, "/exists").methods("GET"_method)([&](const crow::request& req) {
        auto name = req.url_params.get("name");
        if (!name) return crow::response(400, "Falta el parámetro 'name'");
        bool found = docManager.exists(name);
        nlohmann::json j = { {"exists", found} };
        return crow::response(200, j.dump());
    });

    // Endpoint: Descargar documento (GET /download?name=...)
    CROW_ROUTE(app, "/download").methods("GET"_method)([&](const crow::request& req) {
        auto name = req.url_params.get("name");
        if (!name) return crow::response(400, "Falta el parámetro 'name'");
        try {
            auto data = docManager.getDocument(name);
            // Log: imprime los primeros bytes reconstruidos
            std::cout << "[DOWNLOAD] Primeros bytes reconstruidos: ";
            for (size_t i = 0; i < std::min<size_t>(data.size(), 8); ++i) std::cout << std::hex << (int)data[i] << " ";
            std::cout << std::dec << std::endl;
            crow::response res;
            res.code = 200;
            res.body.assign(reinterpret_cast<const char*>(data.data()), data.size());
            res.add_header("Content-Type", "application/octet-stream");
            res.add_header("Content-Disposition", std::string("attachment; filename=") + name);
            return res;
        } catch (const std::exception& ex) {
            return crow::response(404, ex.what());
        }
    });

    // Puerto configurable por argumento
    int port = 18080;
    if (argc > 1) port = std::stoi(argv[1]);
    std::cout << "ControllerNode HTTP API escuchando en puerto " << port << std::endl;
    app.port(port).multithreaded().run();
    return 0;
}
