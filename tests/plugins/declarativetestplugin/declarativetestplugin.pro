CXX_MODULE = location
TARGET  = declarative_location_test
TARGETPATH = QtLocation/test

QT += gui-private qml quick location testlib

INCLUDEPATH += ../../../src/imports/location
INCLUDEPATH += ../../../src/location

HEADERS += qdeclarativepinchgenerator_p.h \
           qdeclarativelocationtestmodel_p.h


SOURCES += locationtest.cpp \
           qdeclarativepinchgenerator.cpp \
           qdeclarativelocationtestmodel.cpp

load(qml_plugin)
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
