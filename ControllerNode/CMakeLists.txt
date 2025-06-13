cmake_minimum_required(VERSION 3.10)
project(ControllerNode)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/../Crow/include)
# Agregar CPR (HTTP client)
find_package(cpr REQUIRED)
# Buscar Boost instalado en el sistema
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(controller_node main.cpp
    core/RAID5Manager.cpp
    core/BlockMap.cpp
    manager/DocumentManager.cpp
    http/DiskNodeHttpClient.cpp
)

add_executable(controller_node_server server.cpp
    core/RAID5Manager.cpp
    core/BlockMap.cpp
    manager/DocumentManager.cpp
    http/DiskNodeHttpClient.cpp
)

target_include_directories(controller_node PRIVATE core manager http)
target_include_directories(controller_node_server PRIVATE core manager http)

target_link_libraries(controller_node PRIVATE cpr::cpr ${Boost_LIBRARIES})
target_link_libraries(controller_node_server PRIVATE cpr::cpr ${Boost_LIBRARIES})
