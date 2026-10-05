
CONFIG += debug_and_release
CONFIG += build_all

CONFIG += ordered
CONFIG += warn_off

TEMPLATE = subdirs

SUBDIRS += \
    qt-plus/qt-plus.pro \
    quick3d/Quick3D.pro \
    quick3dtest/Quick3DTest.pro
