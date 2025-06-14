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

    // Endpoint: Estado de bloques (GET /status)
    CROW_ROUTE(app, "/status").methods("GET"_method)([&]() {
        std::ostringstream html;
        html << "<html><head><title>Estado RAID</title><style>table,th,td{border:1px solid #888;border-collapse:collapse;padding:4px;}th{background:#eee;}</style></head><body>";
        html << "<h2>Estado de bloques en RAID</h2>";
        auto allLocations = blockMap.getAllLocations();
        for (const auto& [docName, locations] : allLocations) {
            html << "<h3>Documento: " << docName << "</h3>";
            html << "<table><tr><th>Stripe</th><th>Nodo</th><th>Block ID</th><th>Paridad</th><th>Estado</th></tr>";
            for (const auto& loc : locations) {
                html << "<tr>";
                html << "<td>" << loc.stripeIndex << "</td>";
                html << "<td>" << loc.diskIndex << "</td>";
                html << "<td>" << loc.blockId << "</td>";
                html << "<td>" << (loc.isParity ? "Sí" : "No") << "</td>";
                auto data = diskClient.readBlock(
                    raid5.getDiskNodes()[loc.diskIndex].ip,
                    raid5.getDiskNodes()[loc.diskIndex].port,
                    loc.blockId
                );
                if (data.has_value())
                    html << "<td style='color:green'>OK</td>";
                else
                    html << "<td style='color:red'>MISSING</td>";
                html << "</tr>";
            }
            html << "</table>";
        }
        html << "</body></html>";
        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "text/html; charset=utf-8");
        res.body = html.str();
        return res;
    });

    // Puerto configurable por argumento
    int port = 18080;
    if (argc > 1) port = std::stoi(argv[1]);
    std::cout << "ControllerNode HTTP API escuchando en puerto " << port << std::endl;
    app.port(port).multithreaded().run();
    return 0;
}
