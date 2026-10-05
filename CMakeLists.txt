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
)

find_package(ZLIB REQUIRED)

add_subdirectory(qt-plus)
add_subdirectory(quick3d)
add_subdirectory(quick3dtest)
