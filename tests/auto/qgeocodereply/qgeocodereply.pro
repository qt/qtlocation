TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeocodereply

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeocodereply.h
SOURCES += tst_qgeocodereply.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

QT += location testlib
