CONFIG += testcase
TARGET = tst_qgeocodingmanager

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
    tst_qgeocodingmanager.h

SOURCES += tst_qgeocodingmanager.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

QT += location testlib
