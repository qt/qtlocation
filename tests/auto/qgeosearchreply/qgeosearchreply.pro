TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeosearchreply

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeosearchreply.h
SOURCES += tst_qgeosearchreply.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

QT += location testlib
