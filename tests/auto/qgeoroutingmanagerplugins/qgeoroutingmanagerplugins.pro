load(qt_plugin)

TARGET = qtgeoservices_routingplugin
QT += location

DESTDIR = $$QT.location.plugins/geoservices

HEADERS += qgeoroutingmanagerengine_test.h \
           qgeoserviceproviderplugin_test.h

SOURCES += qgeoserviceproviderplugin_test.cpp

target.path += $$[QT_INSTALL_PLUGINS]/geoservices
INSTALLS += target

OTHER_FILES += \
    routing_plugin.json
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
