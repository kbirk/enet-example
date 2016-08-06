cmake_minimum_required(VERSION 2.8)
project(enet_example)

message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")

# CMAKE extensions
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${enet_example_SOURCE_DIR}/cmake")

# Required packages
find_package(ENet REQUIRED)
find_package(Epoxy REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Warning pedantic flags for all
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-Wall -Werror -pedantic -Wfatal-errors -O2 ${CMAKE_CXX_FLAGS}")

# Dependencies and include locations
include_directories("include")
include_directories(${ENET_INCLUDE_DIRS})
include_directories(${EPOXY_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})

## Client Executable

# Add source files
set(client_sources
	"src/geometry/Cube"
	"src/geometry/Geometry"
	"src/geometry/Quad"
	"src/geometry/Sphere"
	"src/gl/GLInfo"
	"src/gl/Shader"
	"src/gl/Texture2D"
	"src/gl/TextureCubeMap"
	"src/gl/ElementArrayBufferObject"
	"src/gl/VertexAttributePointer"
	"src/gl/VertexBufferObject"
	"src/gl/VertexArrayObject"
	"src/gl/VertexFragmentShader"
	"src/gl/Uniform"
	"src/gl/UniformDescriptor"
	"src/gl/UniformBlockDescriptor"
	"src/gl/Viewport"
	"src/log/Log"
	"src/net/Client"
	"src/net/Message"
	"src/net/Packet"
	"src/render/Material"
	"src/render/Mesh"
	"src/render/Node"
	"src/render/RenderCommand"
	"src/render/Renderer"
	"src/render/Technique"
	"src/render/Transform"
	"src/serial/Serialization"
	"src/serial/StreamBuffer"
	"src/window/Window"
	"src/Common"
	"src/client")
# Construct the executable
add_executable(client ${client_sources})
# Link the executable to  libraries
target_link_libraries(client
	${ENET_LIBRARIES}
	${EPOXY_LIBRARIES}
	${SDL2_LIBRARY}
	${OPENGL_LIBRARIES})

## Server Executable

# Add source files
set(server_sources
	"src/log/Log"
	"src/net/Server"
	"src/net/Message"
	"src/net/Packet"
	"src/render/Transform"
	"src/serial/Serialization"
	"src/serial/StreamBuffer"
	"src/Common"
	"src/server")
# Construct the executable
add_executable(server ${server_sources})
# Link the executable to  libraries
target_link_libraries(server
	${ENET_LIBRARIES}
	${EPOXY_LIBRARIES}
	${SDL2_LIBRARY}
	${OPENGL_LIBRARIES})

# Copy resource dir to build
file(COPY
	${CMAKE_CURRENT_SOURCE_DIR}/resources/
	DESTINATION
	${CMAKE_CURRENT_BINARY_DIR}/resources/)
