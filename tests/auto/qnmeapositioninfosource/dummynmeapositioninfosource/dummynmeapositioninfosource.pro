TEMPLATE = app
CONFIG+=testcase
QT += network positioning testlib
TARGET = tst_dummynmeapositioninfosource

INCLUDEPATH += ..

HEADERS += ../../utils/qlocationtestutils_p.h \
           ../../qgeopositioninfosource/testqgeopositioninfosource_p.h \
           ../../utils/qnmeaproxyfactory.h

SOURCES += ../../utils/qlocationtestutils.cpp \
           ../../qgeopositioninfosource/testqgeopositioninfosource.cpp \
           ../../utils/qnmeaproxyfactory.cpp \
           tst_dummynmeapositioninfosource.cpp

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
