cmake_minimum_required(VERSION 2.8)
project(enet_example)

message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")

# CMAKE extensions
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${enet_example_SOURCE_DIR}/cmake")

# Required packages
find_package(ENet REQUIRED)

# Warning pedantic flags for all
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic -Wfatal-errors -O2 ${CMAKE_CXX_FLAGS}")

# Dependencies and include locations
include_directories("include")
include_directories(${ENET_INCLUDE_DIRS})

## Client Executable

# Add source files
set(client_sources "src/client.cpp")
# Construct the executable
add_executable(client ${client_sources})
# Link the executable to  libraries
target_link_libraries(client ${ENET_LIBRARIES})

## Server Executable

# Add source files
set(server_sources "src/server.cpp")
# Construct the executable
add_executable(server ${server_sources})
# Link the executable to  libraries
target_link_libraries(server ${ENET_LIBRARIES})
