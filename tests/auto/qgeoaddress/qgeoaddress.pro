TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeoaddress

SOURCES += tst_qgeoaddress.cpp

QT += location testlib
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
