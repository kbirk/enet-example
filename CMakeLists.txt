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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wfatal-errors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Dependencies and include locations
include_directories("include")
include_directories("external/include")
include_directories(${ENET_INCLUDE_DIRS})
include_directories(${EPOXY_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})

## Client Executable

# Add source files
set(client_sources
	"src/enet/ENetClient"
	"src/game/Camera"
	"src/game/Environment"
	"src/game/Frame"
	"src/game/Idle"
	"src/game/Image"
	"src/game/MoveDirection"
	"src/game/MoveTo"
	"src/game/Player"
	"src/game/State"
	"src/game/StateMachine"
	"src/game/StateType"
	"src/game/Terrain"
	"src/geometry/Cube"
	"src/geometry/Geometry"
	"src/geometry/Intersection"
	"src/geometry/Quad"
	"src/geometry/Octree"
	"src/geometry/Sphere"
	"src/geometry/Triangle"
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
	"src/input/Input"
	"src/input/Keyboard"
	"src/input/KeyboardEvent"
	"src/input/Mouse"
	"src/input/MouseEvent"
	"src/input/Window"
	"src/log/Log"
	"src/math/Math"
	"src/math/Transform"
	"src/net/Client"
	"src/net/Message"
	"src/render/Material"
	"src/render/Mesh"
	"src/render/Node"
	"src/render/RenderCommand"
	"src/render/Renderer"
	"src/render/Technique"
	"src/sdl/SDL2Keyboard"
	"src/sdl/SDL2Mouse"
	"src/sdl/SDL2Window"
	"src/serial/Serialization"
	"src/serial/StreamBuffer"
	"src/time/Time"
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
	"src/enet/ENetServer"
	"src/game/Environment"
	"src/game/Frame"
	"src/game/Idle"
	"src/game/Image"
	"src/game/MoveDirection"
	"src/game/MoveTo"
	"src/game/Player"
	"src/game/State"
	"src/game/StateMachine"
	"src/game/StateType"
	"src/game/Terrain"
	"src/geometry/Geometry"
	"src/geometry/Intersection"
	"src/geometry/Octree"
	"src/geometry/Triangle"
	"src/gl/ElementArrayBufferObject"
	"src/gl/GLInfo"
	"src/gl/Texture2D"
	"src/gl/VertexArrayObject"
	"src/gl/VertexAttributePointer"
	"src/gl/VertexBufferObject"
	"src/input/Input"
	"src/log/Log"
	"src/math/Math"
	"src/math/Transform"
	"src/net/Server"
	"src/net/Message"
	"src/serial/Serialization"
	"src/serial/StreamBuffer"
	"src/time/Time"
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
