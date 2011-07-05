load(qt_module)

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = qtgeoservices_qmltestplugin
PLUGIN_TYPE=geoservices
QT += location

include($$QT_SOURCE_TREE/src/plugins/qpluginbase.pri)
# TODO not sure where to put test plugins in Qt 5
DESTDIR = $$QT.location.plugins/geoservices

HEADERS += qgeosearchmanagerengine_test.h \
           qgeoserviceproviderplugin_test.h \
           qgeoroutingmanagerengine_test.h

SOURCES += qgeoserviceproviderplugin_test.cpp


INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps \
               ../../../src/location/landmarks

target.path += $$[QT_INSTALL_PLUGINS]/geoservices
INSTALLS += target
