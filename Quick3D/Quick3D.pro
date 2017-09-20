
QT += core gui network opengl xml positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += QUICK3D_LIB
DEFINES += SFML_SYSTEM_EXPORTS
DEFINES += SFML_WINDOW_EXPORTS
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../qt-plus/source/cpp/Web
INCLUDEPATH += $$PWD/../qt-plus/source/cpp/GeoTools
INCLUDEPATH += $$PWD/../COTS/SFML-1.6/include
INCLUDEPATH += $$PWD/../COTS/unzip11
INCLUDEPATH += $$PWD/../COTS/SFML-1.6/src
LIBS += -lopengl32 -luser32 -lgdi32 -lwinmm
DESTDIR = $$PWD/bin
MOC_DIR = $$PWD/moc
OBJECTS_DIR = $$PWD/obj

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
