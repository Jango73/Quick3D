
CONFIG += debug_and_release
CONFIG += build_all

CONFIG += ordered
CONFIG += warn_off

TEMPLATE = subdirs

SUBDIRS += Quick3D/Quick3D.pro \
    Components_A320/Components_A320.pro \
    Quick3DTest/Quick3DTest.pro \
    VirtualPilot/VirtualPilot.pro
