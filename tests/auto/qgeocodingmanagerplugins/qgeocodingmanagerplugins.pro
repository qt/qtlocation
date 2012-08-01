load(qt_plugin)

TARGET = qtgeoservices_geocodingplugin
QT += location

DESTDIR = $$QT.location.plugins/geoservices

HEADERS += qgeocodingmanagerengine_test.h \
           qgeoserviceproviderplugin_test.h

SOURCES += qgeoserviceproviderplugin_test.cpp

target.path += $$[QT_INSTALL_PLUGINS]/geoservices
INSTALLS += target

OTHER_FILES += \
    geocoding_plugin.json
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
