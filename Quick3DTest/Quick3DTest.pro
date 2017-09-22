#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T20:41:20
#
#-------------------------------------------------

QT += core gui network opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# CONFIG += console

TEMPLATE = app

INCLUDEPATH += $$PWD/../Quick3D/Source
INCLUDEPATH += $$PWD/../qt-plus/source/cpp
INCLUDEPATH += $$PWD/../qt-plus/source/cpp/Web
DEPENDPATH += $$PWD/../Quick3D

DESTDIR = $$PWD/bin
MOC_DIR = $$PWD/moc
OBJECTS_DIR = $$PWD/obj

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

# Copy qt-plus to bin
copyfile = $$PWD/../qt-plus/bin/*.dll
copydest = $$PWD/bin
QMAKE_PRE_LINK += $$QMAKE_COPY /y $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)

# Copy Quick3D to bin
copyfile = $$PWD/../Quick3D/bin/*.dll
copydest = $$PWD/bin
QMAKE_PRE_LINK += $$QMAKE_COPY /y $$quote($$shell_path($$copyfile)) $$quote($$shell_path($$copydest)) $$escape_expand(\\n\\t)

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
