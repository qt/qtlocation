TARGET = qtgeoservices_osm
QT += location-private network

PLUGIN_TYPE = geoservices
load(qt_plugin)

HEADERS += \
    qgeoserviceproviderpluginosm.h \
    qgeotiledmappingmanagerengineosm.h \
    qgeotilefetcherosm.h \
    qgeomapreplyosm.h

SOURCES += \
    qgeoserviceproviderpluginosm.cpp \
    qgeotiledmappingmanagerengineosm.cpp \
    qgeotilefetcherosm.cpp \
    qgeomapreplyosm.cpp

OTHER_FILES += \
    osm_plugin.json

