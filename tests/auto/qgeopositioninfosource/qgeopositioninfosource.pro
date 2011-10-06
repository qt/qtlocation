TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfosource

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           testqgeopositioninfosource_p.h

SOURCES += ../qgeocoordinate/qlocationtestutils.cpp \
           testqgeopositioninfosource.cpp \
           tst_qgeopositioninfosource.cpp

QT += location testlib
