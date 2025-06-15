# Controller Node - Sistema de Archivos Distribuido con RAID 5

Este proyecto implementa un sistema de archivos distribuido tolerante a fallos, basado en RAID 5, compuesto por un Controller Node (C++) y varios Disk Nodes (Python/Flask). Permite almacenar, recuperar y gestionar archivos de manera segura y eficiente.

---

## Función de los archivos principales

### ControllerNode/core/
- **BlockMap.h / BlockMap.cpp**: Gestiona el mapeo entre documentos y la ubicación de sus bloques en los nodos de disco.
- **RAID5Manager.h / RAID5Manager.cpp**: Implementa la lógica de striping y paridad RAID 5, así como la reconstrucción de datos.

### ControllerNode/manager/
- **DocumentManager.h / DocumentManager.cpp**: Orquesta la gestión de documentos: agregar, eliminar, buscar y recuperar archivos usando BlockMap y RAID5Manager.

### ControllerNode/http/
- **DiskNodeHttpClient.h / DiskNodeHttpClient.cpp**: Cliente HTTP para enviar y recuperar bloques a/desde los Disk Nodes.

### ControllerNode/
- **server.cpp**: Implementa la API HTTP principal usando Crow.
- **main.cpp**: Ejemplo de uso y pruebas de tolerancia a fallos.

### tecmfs_disk/
- **disk_node.py**: Servidor Flask que representa un nodo de disco, almacena y recupera bloques.
- **utils/config_parser.py**: Carga la configuración de cada nodo de disco desde XML.

### pdf_client/
- **pdf_manager.py**: Cliente gráfico (Tkinter) para gestionar archivos PDF en el sistema distribuido.

### Scripts auxiliares
- **run_all_disk_nodes.sh**: Lanza todos los Disk Nodes automáticamente.
- **stop_all_disk_nodes.sh**: Detiene todos los Disk Nodes.
- **clean_old_blocks.sh**: Limpia los bloques antiguos de los nodos de disco.

---

## Estructuras de datos principales
- **BlockLocation**: Indica en qué nodo, stripe y posición se encuentra un bloque (y si es de paridad).
- **Block**: Representa un bloque de datos o de paridad.
- **DocInfo**: Almacena la lista de ubicaciones de bloques y el tamaño original de un documento.
- **DiskNodeInfo**: Contiene la IP y puerto de un nodo de disco.

---

## Integración y dependencias
- **DocumentManager** depende de **BlockMap**, **RAID5Manager** y **DiskNodeHttpClient** para gestionar documentos y bloques.
- **BlockMap** mantiene el registro de la ubicación de todos los bloques y su paridad.
- **RAID5Manager** define los parámetros globales de RAID 5 (tamaño de bloque, número de nodos) y la lógica de paridad.
- **DiskNodeHttpClient** se encarga de la comunicación HTTP con los Disk Nodes.
- **disk_node.py** utiliza Flask para exponer endpoints de lectura/escritura de bloques y se configura mediante **config_parser.py**.
- El cliente gráfico se comunica con la API HTTP del Controller Node.

---

## Requisitos previos
- **C++17** o superior
- **Python 3.7+**
- **pip** para instalar dependencias de Python
- **Crow**, **nlohmann/json**, **cppcodec**, **CPR** (instalados vía vcpkg o gestor preferido)
- **Flask** para los Disk Nodes

## Instalación de dependencias

### Dependencias de C++ (ControllerNode)
Instalar vcpkg y ejecutar:
```bash
vcpkg install crow nlohmann-json cppcodec cpr
```
O instalar manualmente las bibliotecas equivalentes.

### Dependencias de Python (Disk Nodes)
Desde la raíz del proyecto:
```bash
pip install flask
```

## Inicialización y limpieza
Antes de iniciar el sistema, se recomienda limpiar los bloques antiguos:
```bash
bash clean_old_blocks.sh
```

## Ejecución de los Disk Nodes

### Opción 1: Ejecutar todos los Disk Nodes automáticamente
Para lanzar los 4 Disk Nodes configurados:
```bash
bash run_all_disk_nodes.sh
```

### Opción 2: Ejecutar los Disk Nodes uno por uno (en diferentes terminales)
```bash
python3 tecmfs_disk/disk_node.py tecmfs_disk/disk0/config0.xml
python3 tecmfs_disk/disk_node.py tecmfs_disk/disk1/config1.xml
python3 tecmfs_disk/disk_node.py tecmfs_disk/disk2/config2.xml
python3 tecmfs_disk/disk_node.py tecmfs_disk/disk3/config3.xml
```

## Ejecución del Controller Node
Para compilar y ejecutar el Controller Node:
```bash
cd ControllerNode
mkdir -p build && cd build
cmake ..
make
./controller_node
```
Por defecto, el Controller escucha en el puerto 18080. El puerto puede modificarse pasando el valor como argumento.

## Uso básico de la API

### Subir un archivo
```bash
echo "Hola, mundo RAID 5" > archivo.txt
curl -X POST "http://localhost:18080/add?name=archivo.txt" --data-binary @archivo.txt
```

### Descargar un archivo
```bash
curl -X GET "http://localhost:18080/download?name=archivo.txt" -o archivo_descargado.txt
```

### Eliminar un archivo
```bash
curl -X DELETE "http://localhost:18080/delete?name=archivo.txt"
```

### Verificar si existe un archivo
```bash
curl -X GET "http://localhost:18080/exists?name=archivo.txt"
```

### Ver estado de los bloques y nodos (dashboard)
Abrir en un navegador:
[http://localhost:18080/status](http://localhost:18080/status)

## Notas adicionales
- Para detener todos los Disk Nodes:
  ```bash
  bash stop_all_disk_nodes.sh
  ```
- El sistema es tolerante a fallos: si uno de los Disk Nodes deja de funcionar, los archivos aún pueden recuperarse.
- Se soportan archivos binarios (PDF, imágenes, etc.).
- Se Incluye un cliente gráfico en `pdf_client/pdf_manager.py` para la gestión de archivos PDF.