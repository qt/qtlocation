TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeorectangle

SOURCES += \
    tst_qgeorectangle.cpp

QT += location testlib
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
