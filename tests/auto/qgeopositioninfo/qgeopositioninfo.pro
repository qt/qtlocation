TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfo

SOURCES += tst_qgeopositioninfo.cpp

QT += location testlib
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
