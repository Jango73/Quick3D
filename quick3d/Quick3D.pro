
win32 {
    QT += core gui network opengl xml positioning
} else {
    QT += core gui network opengl xml
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += QUICK3D_LIB
DEFINES += SFML_SYSTEM_EXPORTS
DEFINES += SFML_WINDOW_EXPORTS

# Dependencies
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../qt-plus/source/cpp/web
INCLUDEPATH += $$PWD/../qt-plus/source/cpp/geotools
INCLUDEPATH += $$PWD/../third/unzip11

win32 {
    INCLUDEPATH += $$PWD/../third/SFML-1.6/include
    INCLUDEPATH += $$PWD/../third/SFML-1.6/src
}

INCLUDEPATH += $$PWD/source
INCLUDEPATH += $$PWD/source/animation
INCLUDEPATH += $$PWD/source/base
INCLUDEPATH += $$PWD/source/components
INCLUDEPATH += $$PWD/source/input
INCLUDEPATH += $$PWD/source/math
INCLUDEPATH += $$PWD/source/mesh
INCLUDEPATH += $$PWD/source/render
INCLUDEPATH += $$PWD/source/terrain
INCLUDEPATH += $$PWD/source/utils
INCLUDEPATH += $$PWD/source/zip

win32 {
    LIBS += -lopengl32 -luser32 -lgdi32 -lwinmm
}

# Directories
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/obj
MOC_DIR = $$PWD/moc
RCC_DIR = $$PWD/rcc
UI_DIR = $$PWD/ui

# C++ Flags
QMAKE_CXXFLAGS += -Wno-invalid-offsetof
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-reorder

# Target
CONFIG(debug, debug|release) {
    TARGET = Quick3Dd
} else {
    TARGET = Quick3D
}

include(Quick3D.pri)

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plusd
} else {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plus
}

# Help
helpfile = Quick3D.qdocconf

QMAKE_POST_LINK += qdoc $$quote($$shell_path($$helpfile)) $$escape_expand(\\n\\t)
