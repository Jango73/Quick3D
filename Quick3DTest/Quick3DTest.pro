#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T20:41:20
#
#-------------------------------------------------

QT += core gui network opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Quick3DTest
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

# Libraries
CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../Quick3D/debug/ -lQuick3D
} else {
    LIBS += -L$$OUT_PWD/../Quick3D/release/ -lQuick3D
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
    main.cpp

HEADERS  += \
    Quick3DTest.h

DISTFILES +=

RESOURCES += \
    Quick3DTest.qrc

FORMS += \
    ExportDialog.ui \
    Quick3DTest.ui
