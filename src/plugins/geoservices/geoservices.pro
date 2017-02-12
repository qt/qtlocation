TEMPLATE = subdirs

SUBDIRS = nokia osm mapbox esri

qtConfig(opengl) {
    linux|android: {
        equals(QT_GCC_MAJOR_VERSION, 4): greaterThan(QT_GCC_MINOR_VERSION, 8) {
            SUBDIRS += mapboxgl
        }

        greaterThan(QT_GCC_MAJOR_VERSION, 4) {
            SUBDIRS += mapboxgl
        }
    }

    ios|macos: {
        SUBDIRS += mapboxgl
    }
}
