TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeomaneuver

# Input
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeomaneuver.h
SOURCES += tst_qgeomaneuver.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

QT += location testlib
