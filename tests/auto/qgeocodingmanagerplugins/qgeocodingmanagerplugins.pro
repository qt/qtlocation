TARGET = qtgeoservices_geocodingplugin
QT += location

PLUGIN_TYPE = geoservices
load(qt_plugin)

HEADERS += qgeocodingmanagerengine_test.h \
           qgeoserviceproviderplugin_test.h

SOURCES += qgeoserviceproviderplugin_test.cpp

OTHER_FILES += \
    geocoding_plugin.json
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
