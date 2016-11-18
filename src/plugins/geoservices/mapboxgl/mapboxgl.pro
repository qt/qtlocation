TARGET = qtgeoservices_mapboxgl

QT += \
    quick-private \
    location-private \
    positioning-private \
    network

HEADERS += \
    qgeoserviceproviderpluginmapboxgl.h \
    qgeomappingmanagerenginemapboxgl.h \
    qgeomapmapboxgl.h \
    qsgmapboxglnode.h

SOURCES += \
    qgeoserviceproviderpluginmapboxgl.cpp \
    qgeomappingmanagerenginemapboxgl.cpp \
    qgeomapmapboxgl.cpp \
    qsgmapboxglnode.cpp

OTHER_FILES += \
    mapboxgl_plugin.json

INCLUDEPATH += ../3rdparty/mapbox-gl-native/platform/qt/include

LIBS_PRIVATE += -L$$OUT_PWD/../3rdparty/mapbox-gl-native/ -lqmapboxgl

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryMapboxGL
load(qt_plugin)
