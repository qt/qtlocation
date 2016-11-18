TEMPLATE = subdirs

SUBDIRS =

linux|android: {
    equals(QT_GCC_MAJOR_VERSION, 4): greaterThan(QT_GCC_MINOR_VERSION, 8) {
        SUBDIRS += mapbox-gl-native
    }

    greaterThan(QT_GCC_MAJOR_VERSION, 4) {
        SUBDIRS += mapbox-gl-native
    }
}

ios|macos: {
    SUBDIRS += mapbox-gl-native
}
