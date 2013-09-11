TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoareamonitor

SOURCES += tst_qgeoareamonitor.cpp

QT += positioning testlib
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
