TEMPLATE = app
CONFIG+=testcase
QT += network location testlib
TARGET = tst_qnmeapositioninfosource_realtime

INCLUDEPATH += ..

HEADERS += ../../qgeocoordinate/qlocationtestutils_p.h \
           ../../qgeopositioninfosource/testqgeopositioninfosource_p.h \
           ../qnmeapositioninfosourceproxyfactory.h \
           ../tst_qnmeapositioninfosource.h

SOURCES += ../../qgeocoordinate/qlocationtestutils.cpp \
           ../../qgeopositioninfosource/testqgeopositioninfosource.cpp \
           ../qnmeapositioninfosourceproxyfactory.cpp \
           ../tst_qnmeapositioninfosource.cpp \
           tst_qnmeapositioninfosource_realtime.cpp
