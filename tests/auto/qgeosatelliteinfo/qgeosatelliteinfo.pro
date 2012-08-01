TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfo

SOURCES += tst_qgeosatelliteinfo.cpp

QT += testlib location
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
