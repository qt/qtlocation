CONFIG += testcase
TARGET = tst_qgeolocation

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeolocation.h
SOURCES += tst_qgeolocation.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

QT += location testlib
