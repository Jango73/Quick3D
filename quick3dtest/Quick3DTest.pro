#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T20:41:20
#
#-------------------------------------------------

QT += core gui network opengl xml testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# CONFIG += console

TEMPLATE = app

# Dependencies
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../qt-plus/source/cpp/web
INCLUDEPATH += $$PWD/../quick3d/source
INCLUDEPATH += $$PWD/../quick3d/source/animation
INCLUDEPATH += $$PWD/../quick3d/source/base
INCLUDEPATH += $$PWD/../quick3d/source/components
INCLUDEPATH += $$PWD/../quick3d/source/input
INCLUDEPATH += $$PWD/../quick3d/source/math
INCLUDEPATH += $$PWD/../quick3d/source/mesh
INCLUDEPATH += $$PWD/../quick3d/source/render
INCLUDEPATH += $$PWD/../quick3d/source/terrain
INCLUDEPATH += $$PWD/../quick3d/source/utils

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
    LIBS += -L$$PWD/../quick3d/bin -lQuick3Dd
} else {
    LIBS += -L$$PWD/../quick3d/bin -lQuick3D
}

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plusd
} else {
    LIBS += -L$$PWD/../qt-plus/bin -lqt-plus
}

# Code
SOURCES += \
    Quick3DTest.cpp \
    tests/CTestFixture.cpp \
    tests/CTestMath.cpp \
    tests/CTestCamera.cpp \
    tests/CTestGeo.cpp \
    tests/CTestPhysics.cpp \
    tests/CTestVehicle.cpp \
    tests/CTestScene.cpp \
    tests/CTestQuick3D.cpp \
    main.cpp

HEADERS  += \
    Quick3DTest.h \
    tests/CTestFixture.h \
    tests/CTestMath.h \
    tests/CTestCamera.h \
    tests/CTestGeo.h \
    tests/CTestPhysics.h \
    tests/CTestVehicle.h \
    tests/CTestScene.h \
    tests/CTestQuick3D.h

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
    copyfile = $$PWD/../quick3d/bin/*.dll
    copydest = $$PWD/bin
} else {
    copyfile = $$PWD/../quick3d/bin/*.so
    copydest = $$PWD/bin
}

QMAKE_PRE_LINK += $$QMAKE_COPY $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)
