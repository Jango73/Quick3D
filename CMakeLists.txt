cmake_minimum_required(VERSION 3.16)

project(Quick3D LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    Xml
    OpenGL
    OpenGLWidgets
    SerialPort
    Multimedia
    Positioning
    Qml
    Test
)

find_package(ZLIB REQUIRED)

add_subdirectory(qt-plus)

# JoltPhysics (submodule third/JoltPhysics, pinned tag v5.6.0).
# Included from Build/ so only the Jolt static library target is defined.
# INTERPROCEDURAL_OPTIMIZATION is forced off: Jolt would otherwise push LTO
# into our own scope. DOUBLE_PRECISION stays off: geo coordinates convert
# to local float frames at the mapping layer. GPU compute backends stay off:
# the engine uses CPU physics only; leaving JPH_USE_VK on makes the build
# depend on the runner having both libvulkan and dxc/glslc (CI failure
# 106885020982: libvulkan found, Vulkan_GLSLC_EXECUTABLE-NOTFOUND).
set(INTERPROCEDURAL_OPTIMIZATION OFF)
set(ENABLE_INSTALL OFF)
set(ENABLE_ALL_WARNINGS OFF)
set(JPH_USE_DX12 OFF CACHE BOOL "" FORCE)
set(JPH_USE_VK OFF CACHE BOOL "" FORCE)
set(JPH_USE_MTL OFF CACHE BOOL "" FORCE)
set(JPH_USE_CPU_COMPUTE ON CACHE BOOL "" FORCE)
add_subdirectory(third/JoltPhysics/Build)
# Static Jolt objects link into the shared Quick3D library: require PIC.
set_target_properties(Jolt PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(quick3d)
add_subdirectory(quick3dtest)
