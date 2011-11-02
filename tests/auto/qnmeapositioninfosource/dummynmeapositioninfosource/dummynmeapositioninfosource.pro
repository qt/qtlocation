TEMPLATE = app
CONFIG+=testcase
QT += network location testlib
TARGET = tst_dummynmeapositioninfosource

INCLUDEPATH += ..

HEADERS += ../../qgeocoordinate/qlocationtestutils_p.h \
           ../../qgeopositioninfosource/testqgeopositioninfosource_p.h \
           ../qnmeapositioninfosourceproxyfactory.h

SOURCES += ../../qgeocoordinate/qlocationtestutils.cpp \
           ../../qgeopositioninfosource/testqgeopositioninfosource.cpp \
           ../qnmeapositioninfosourceproxyfactory.cpp \
           tst_dummynmeapositioninfosource.cpp
