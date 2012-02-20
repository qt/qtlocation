load(qt_plugin)

TARGET = qtgeoservices_placesplugin_unsupported
QT += location

DESTDIR = $$QT.location.plugins/geoservices

HEADERS += qgeoserviceproviderplugin_test.h

SOURCES += qgeoserviceproviderplugin_test.cpp

target.path += $$[QT_INSTALL_PLUGINS]/geoservices
INSTALLS += target
