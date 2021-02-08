TEMPLATE = app
CONFIG+=testcase
testcase.timeout = 400 # this test is slow
QT += network positioning testlib
TARGET = tst_qnmeapositioninfosource_realtime_generic

INCLUDEPATH += ..

HEADERS += ../../utils/qlocationtestutils_p.h \
           ../../qgeopositioninfosource/testqgeopositioninfosource_p.h \
           ../../utils/qnmeaproxyfactory.h \
           ../tst_qnmeapositioninfosource.h

SOURCES += ../../utils/qlocationtestutils.cpp \
           ../../qgeopositioninfosource/testqgeopositioninfosource.cpp \
           ../../utils/qnmeaproxyfactory.cpp \
           ../tst_qnmeapositioninfosource.cpp \
           tst_qnmeapositioninfosource_realtime_generic.cpp

CONFIG -= app_bundle

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
