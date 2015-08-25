TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeocameracapabilities

INCLUDEPATH += ../../../src/location/maps

SOURCES += tst_qgeocameracapabilities.cpp

QT += location positioning-private testlib
