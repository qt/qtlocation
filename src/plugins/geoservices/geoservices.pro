TEMPLATE = subdirs

SUBDIRS = nokia osm mapbox esri

linux|android: {
    equals(QT_GCC_MAJOR_VERSION, 4): greaterThan(QT_GCC_MINOR_VERSION, 8) {
        SUBDIRS += mapboxgl 3rdparty
        mapboxgl.depends += 3rdparty
    }

    greaterThan(QT_GCC_MAJOR_VERSION, 4) {
        SUBDIRS += mapboxgl 3rdparty
        mapboxgl.depends += 3rdparty
    }
}

ios|macos: {
    SUBDIRS += mapboxgl 3rdparty
    mapboxgl.depends += 3rdparty
}
