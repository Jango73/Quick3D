#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T20:41:20
#
#-------------------------------------------------

QT += core gui network opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# CONFIG += console

TEMPLATE = app

# Dependencies
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../qt-plus/source/cpp/Web
INCLUDEPATH += $$PWD/../Quick3D/Source
INCLUDEPATH += $$PWD/../Quick3D/Source/Animation
INCLUDEPATH += $$PWD/../Quick3D/Source/Base
INCLUDEPATH += $$PWD/../Quick3D/Source/Components
INCLUDEPATH += $$PWD/../Quick3D/Source/Input
INCLUDEPATH += $$PWD/../Quick3D/Source/Math
INCLUDEPATH += $$PWD/../Quick3D/Source/Mesh
INCLUDEPATH += $$PWD/../Quick3D/Source/Render
INCLUDEPATH += $$PWD/../Quick3D/Source/Terrain
INCLUDEPATH += $$PWD/../Quick3D/Source/Utils

DEPENDPATH += $$PWD/../Quick3D

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
    TARGET = Quick3DTestd
} else {
    TARGET = Quick3DTest
}

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../Quick3D/bin -lQuick3Dd
} else {
    LIBS += -L$$PWD/../Quick3D/bin -lQuick3D
}

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plusd
} else {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plus
}

# Code
SOURCES += \
    Quick3DTest.cpp \
    CUnitTests.cpp \
    main.cpp

HEADERS  += \
    Quick3DTest.h \
    CUnitTests.h

RESOURCES += \
    Quick3DTest.qrc

FORMS += \
    ExportDialog.ui \
    Quick3DTest.ui

# Copy qt-plus to bin
win32 {
    copyfile = $$PWD/../qt-plus/bin/*.dll
    copydest = $$PWD/bin
} else {
    copyfile = $$PWD/../qt-plus/bin/*.so
    copydest = $$PWD/bin
}

QMAKE_PRE_LINK += $$QMAKE_COPY $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)

# Copy Quick3D to bin
win32 {
    copyfile = $$PWD/../Quick3D/bin/*.dll
    copydest = $$PWD/bin
} else {
    copyfile = $$PWD/../Quick3D/bin/*.so
    copydest = $$PWD/bin
}

QMAKE_PRE_LINK += $$QMAKE_COPY $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)
