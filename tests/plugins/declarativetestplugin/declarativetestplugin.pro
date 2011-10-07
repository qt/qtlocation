TARGET  = declarative_location_test
TARGETPATH = QtLocation/test

include(../../../src/imports/location/qlocationimport.pri)

QT += declarative location widgets

DESTDIR = $$QT.location.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INCLUDEPATH += ../../../src/imports/location
INCLUDEPATH += ../../../src/location

HEADERS += qdeclarativepinchgenerator_p.h \
           qdeclarativelocationtestmodel_p.h \
           ../../../src/imports/location/qdeclarativecoordinate_p.h


SOURCES += locationtest.cpp \
           qdeclarativepinchgenerator.cpp \
           qdeclarativelocationtestmodel.cpp \
           ../../../src/imports/location/qdeclarativecoordinate.cpp

# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
