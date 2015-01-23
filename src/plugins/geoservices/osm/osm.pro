TARGET = qtgeoservices_osm
QT += location-private positioning-private network

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryOsm
load(qt_plugin)

HEADERS += \
    qgeoserviceproviderpluginosm.h \
    qgeotiledmappingmanagerengineosm.h \
    qgeotiledmapdataosm.h \
    qgeotilefetcherosm.h \
    qgeomapreplyosm.h \
    qgeocodingmanagerengineosm.h \
    qgeocodereplyosm.h \
    qgeoroutingmanagerengineosm.h \
    qgeoroutereplyosm.h \
    qplacemanagerengineosm.h \
    qplacesearchreplyimpl.h \
    qplacecategoriesreplyimpl.h

SOURCES += \
    qgeoserviceproviderpluginosm.cpp \
    qgeotiledmappingmanagerengineosm.cpp \
    qgeotiledmapdataosm.cpp \
    qgeotilefetcherosm.cpp \
    qgeomapreplyosm.cpp \
    qgeocodingmanagerengineosm.cpp \
    qgeocodereplyosm.cpp \
    qgeoroutingmanagerengineosm.cpp \
    qgeoroutereplyosm.cpp \
    qplacemanagerengineosm.cpp \
    qplacesearchreplyimpl.cpp \
    qplacecategoriesreplyimpl.cpp

OTHER_FILES += \
    osm_plugin.json

