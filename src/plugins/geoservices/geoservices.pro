TEMPLATE = subdirs

SUBDIRS = nokia osm mapbox esri

defineTest(supportsMapboxGL) {
    linux|android {
        equals(QT_GCC_MAJOR_VERSION, 4): greaterThan(QT_GCC_MINOR_VERSION, 8) {
            return(true)
        }

        greaterThan(QT_GCC_MAJOR_VERSION, 4) {
            return(true)
        }
    }

    ios|macos {
        return(true)
    }

    return(false)
}

supportsMapboxGL() {
    !exists(../../3rdparty/mapbox-gl-native/CMakeLists.txt) {
        warning("Submodule mapbox-gl-native does not exist. Run 'git submodule update --init' on qtlocation.")
    } else {
        SUBDIRS += mapboxgl ../../3rdparty/mapbox-gl-native
        mapboxgl.depends = ../../3rdparty/mapbox-gl-native
    }
}
