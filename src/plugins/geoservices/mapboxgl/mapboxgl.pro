TARGET = qtgeoservices_mapboxgl

QT += \
    quick-private \
    location-private \
    positioning-private \
    network \
    opengl \
    sql

HEADERS += \
    qgeoserviceproviderpluginmapboxgl.h \
    qgeomappingmanagerenginemapboxgl.h \
    qgeomapmapboxgl.h \
    qgeomapmapboxgl_p.h \
    qmapboxglstylechange_p.h \
    qsgmapboxglnode.h

SOURCES += \
    qgeoserviceproviderpluginmapboxgl.cpp \
    qgeomappingmanagerenginemapboxgl.cpp \
    qgeomapmapboxgl.cpp \
    qmapboxglstylechange.cpp \
    qsgmapboxglnode.cpp

RESOURCES += mapboxgl.qrc

OTHER_FILES += \
    mapboxgl_plugin.json

INCLUDEPATH += ../../../3rdparty/mapbox-gl-native/platform/qt/include

LIBS_PRIVATE += -L$$OUT_PWD/../../../3rdparty/mapbox-gl-native/ -lqmapboxgl

qtConfig(icu) {
    include(../../../3rdparty/icu_dependency.pri)
}

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryMapboxGL
load(qt_plugin)
