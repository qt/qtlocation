TEMPLATE = app
!no_system_tests:CONFIG += testcase
TARGET=tst_qgeosatelliteinfosource

SOURCES += tst_qgeosatelliteinfosource.cpp \
            testqgeosatelliteinfosource.cpp \
            ../qgeocoordinate/qlocationtestutils.cpp

HEADERS += testqgeosatelliteinfosource_p.h \
            ../qgeocoordinate/qlocationtestutils_p.h

QT += location testlib

!isEmpty(QT.jsondb.name):!isEmpty(QT.jsonstream.name):!simulator:DEFINES += NPE_BACKEND
