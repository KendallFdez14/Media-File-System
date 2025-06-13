// Punto de entrada del Controller Node
#include "core/BlockMap.h"
#include "core/RAID5Manager.h"
#include "manager/DocumentManager.h"
#include "http/DiskNodeHttpClient.h"
#include <iostream>
#include <vector>

int main() {
    BlockMap blockMap;
    RAID5Manager raid5(4, 4096); // 4 nodos, bloques de 4096 bytes
    // Agrega los nodos (ajusta IP/puerto según tus Disk Nodes)
    raid5.addDiskNode({"127.0.0.1", 8000});
    raid5.addDiskNode({"127.0.0.1", 8001});
    raid5.addDiskNode({"127.0.0.1", 8002});
    raid5.addDiskNode({"127.0.0.1", 8003});

    DiskNodeHttpClient diskClient;
    DocumentManager docManager(blockMap, raid5, diskClient);

    // Crea datos de prueba
    std::vector<uint8_t> data(8192, 42); // 8 KiB de valor 42

    // Agrega documento (lo envía a los Disk Nodes)
    std::cout << "Guardando documento de prueba en los Disk Nodes..." << std::endl;
    docManager.addDocument("testfile", data);
    std::cout << "Documento guardado.\n";

    std::cout << "\n*** AHORA puedes detener uno de los Disk Nodes (por ejemplo, cierra la terminal de disk1 o usa stop_all_disk_nodes.sh) ***\n";
    std::cout << "Presiona ENTER para continuar con la recuperación..." << std::endl;
    std::cin.get();

    // Recupera documento
    try {
        auto recovered = docManager.getDocument("testfile");
        std::cout << "Recuperados " << recovered.size() << " bytes" << std::endl;
        if (recovered == data) {
            std::cout << "¡Recuperación exitosa incluso con un Disk Node caído!" << std::endl;
        } else {
            std::cout << "Error: los datos recuperados no coinciden." << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cout << "Error al recuperar el documento: " << ex.what() << std::endl;
    }
    return 0;
}
