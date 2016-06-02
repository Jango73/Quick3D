#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T20:41:20
#
#-------------------------------------------------

QT       += core gui network opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Components_A320
TEMPLATE = lib
DEFINES += COMPONENTS_A320_LIB
INCLUDEPATH += $$PWD/../Quick3D/Source
DEPENDPATH += $$PWD/../Quick3D
CONFIG(debug, debug|release): DESTDIR = ../VirtualPilot/debug/Plugins
CONFIG(release, debug|release): DESTDIR = ../VirtualPilot/release/Plugins

# C++ Flags
QMAKE_CXXFLAGS += -Wno-invalid-offsetof
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-reorder

# Libraries
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Quick3D/release/ -lQuick3D
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Quick3D/debug/ -lQuick3D
else:unix: LIBS += -L$$OUT_PWD/../Quick3D/ -lQuick3D

# Code
HEADERS += \
    CAirbusADIRU.h \
    CAirbusAOASensor.h \
    CAirbusComponent.h \
    CAirbusController.h \
    CAirbusData.h \
    CAirbusDataSupplier.h \
    CAirbusDMC.h \
    CAirbusELAC.h \
    CAirbusEngineGenerator.h \
    CAirbusFAC.h \
    CAirbusFADEC.h \
    CAirbusFCU.h \
    CAirbusFlightComputer.h \
    CAirbusFlightPlan.h \
    CAirbusFMGC.h \
    CAirbusPitotPort.h \
    CAirbusSEC.h \
    CAirbusStaticPort.h \
    Components_A320.h \
    components_a320_global.h \
    Constants.h

SOURCES += \
    CAirbusADIRU.cpp \
    CAirbusAOASensor.cpp \
    CAirbusComponent.cpp \
    CAirbusController.cpp \
    CAirbusData.cpp \
    CAirbusDataSupplier.cpp \
    CAirbusDMC.cpp \
    CAirbusDMC_EWD.cpp \
    CAirbusDMC_ND.cpp \
    CAirbusDMC_PFD.cpp \
    CAirbusDMC_SD.cpp \
    CAirbusELAC.cpp \
    CAirbusEngineGenerator.cpp \
    CAirbusFAC.cpp \
    CAirbusFADEC.cpp \
    CAirbusFCU.cpp \
    CAirbusFlightComputer.cpp \
    CAirbusFlightPlan.cpp \
    CAirbusFMGC.cpp \
    CAirbusPitotPort.cpp \
    CAirbusSEC.cpp \
    CAirbusStaticPort.cpp \
    Components_A320.cpp

RESOURCES += \
    A320.qrc
