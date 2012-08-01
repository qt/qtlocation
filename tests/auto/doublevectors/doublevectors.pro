TEMPLATE = app
CONFIG += testcase
TARGET = tst_doublevectors
INCLUDEPATH += ../../../src/location/maps

SOURCES += tst_doublevectors.cpp \
    ../../../src/location/maps/qdoublevector3d.cpp \
    ../../../src/location/maps/qdoublevector2d.cpp

QT += location testlib
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
