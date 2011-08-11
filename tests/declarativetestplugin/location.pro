TARGET  = declarative_location_test
TARGETPATH = Qt/location/test

include(../../src/imports/qimportbase.pri)

QT += declarative location

DESTDIR = $$QT.location.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INCLUDEPATH += ../../src/imports/location
INCLUDEPATH += ../../src/location

# On some platforms, build both versions because debug and release
# versions are incompatible
#win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release

HEADERS += qdeclarativepinchgenerator_p.h \
           qdeclarativelocationtestmodel_p.h \
           ../../src/imports/location/qdeclarativecoordinate_p.h


SOURCES += locationtest.cpp \
           qdeclarativepinchgenerator.cpp \
           qdeclarativelocationtestmodel.cpp \
           ../../src/imports/location/qdeclarativecoordinate.cpp

# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
