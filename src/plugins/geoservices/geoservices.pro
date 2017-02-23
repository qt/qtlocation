TEMPLATE = subdirs

SUBDIRS = nokia osm mapbox esri

qtHaveModule(opengl) {
    linux|android: {
        equals(QT_GCC_MAJOR_VERSION, 4): greaterThan(QT_GCC_MINOR_VERSION, 8) {
            SUBDIRS += mapboxgl ../../3rdparty/mapbox-gl-native
            mapboxgl.depends = ../../3rdparty/mapbox-gl-native
        }

        greaterThan(QT_GCC_MAJOR_VERSION, 4) {
            SUBDIRS += mapboxgl ../../3rdparty/mapbox-gl-native
            mapboxgl.depends = ../../3rdparty/mapbox-gl-native
        }
    }

    ios|macos: {
        SUBDIRS += mapboxgl ../../3rdparty/mapbox-gl-native
        mapboxgl.depends = ../../3rdparty/mapbox-gl-native
    }
}
