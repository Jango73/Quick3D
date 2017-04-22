
CONFIG += debug_and_release
CONFIG += build_all

CONFIG += ordered
CONFIG += warn_off

TEMPLATE = subdirs

SUBDIRS += \
    Quick3D/Quick3D.pro \
    Quick3DTest/Quick3DTest.pro
