# Controller Node - Estructuras de Datos y Módulos
Este directorio contiene las estructuras de datos y módulos principales para la gestión de bloques, nodos y lógica RAID 5 en el Controller Node.

## Estructuras de Datos
- **BlockMetadata.h**: Define la estructura para almacenar los metadatos de cada documento, incluyendo nombre, tipo, tamaño, lista de bloques y su ubicación (incluyendo información de paridad).
- **DiskNodeState.h**: Define la estructura para mantener el estado de cada Disk Node, incluyendo IP, puerto, capacidad, espacio disponible, estado y los bloques almacenados.
- **BlockMap.h**: Proporciona un mapeo eficiente entre documentos, bloques y nodos, permitiendo saber dónde está almacenado cada bloque y su paridad.

## Módulos Internos
- **DocumentManager.h**: Gestión de documentos (agregar, eliminar, buscar, descargar).
- **RAID5Manager.h**: Lógica de striping y cálculo/recuperación de paridad RAID 5.
- **DiskNodeHttpClient.h**: Cliente HTTP para comunicación con los Disk Nodes (almacenamiento y recuperación de bloques, manejo de errores y reintentos).
- **Monitor.h**: Recolección y presentación del estado de los Disk Nodes y bloques.

## Librerías Externas Necesarias

- **Crow**: Framework HTTP moderno y sencillo para C++ (usado para exponer la API HTTP del Controller Node).
- **nlohmann/json**: Para parseo y serialización de JSON. 
- **cppcodec**: Para codificación y decodificación base64 eficiente y segura. 

### Notas de Producción
- Asegúrate de que el include path de tu proyecto apunte a las carpetas donde están los headers de estas librerías.
- cppcodec y nlohmann/json son header-only, no requieren linking especial.
- Crow es header-only, pero requiere linking con Boost y OpenSSL si usas HTTPS.

## Comunicación HTTP con Disk Nodes

- **Librería elegida para cliente:** Boost.Beast (solo para comunicación con Disk Nodes, no para el servidor HTTP principal).
- **Protocolo:** JSON sobre HTTP.
    - `POST /block` para almacenar bloques. Body: `{ "blockId": "...", "data": "...", "isParity": true/false }`
    - `GET /block/{blockId}` para recuperar bloques.
    - Respuestas en JSON con estado y datos o mensaje de error.

## Lógica RAID 5
- División de documentos en bloques de tamaño fijo (4096 bytes por defecto).
- Cálculo de paridad XOR para cada grupo de bloques (N-1 datos, 1 paridad).
- Distribución rotatoria de la paridad entre los 4 Disk Nodes.
- Recuperación de bloques perdidos usando XOR de los bloques restantes y la paridad.
- Lectura tolerante a fallos: si un Disk Node falla, el Controller puede reconstruir el bloque perdido.

## Integración y dependencias
- DocumentManager depende de BlockMap, RAID5Manager y DiskNodeHttpClient.
- Monitor depende de BlockMap, RAID5Manager y DiskNodeHttpClient.
- BlockMap provee utilidades para registrar y consultar la ubicación de bloques y paridad.
- RAID5Manager define los parámetros globales de RAID 5 (BLOCK_SIZE, NUM_DISK_NODES). 