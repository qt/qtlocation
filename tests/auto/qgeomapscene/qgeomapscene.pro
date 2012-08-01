CONFIG += testcase
TARGET = tst_qgeomapscene

INCLUDEPATH += ../../../src/location/maps

SOURCES += tst_qgeomapscene.cpp

QT += location 3d testlib
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
