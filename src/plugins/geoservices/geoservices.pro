TEMPLATE = subdirs

SUBDIRS = nokia osm mapbox esri

qtConfig(c++14):!win32 {
    !exists(../../3rdparty/mapbox-gl-native/CMakeLists.txt) {
        warning("Submodule mapbox-gl-native does not exist. Run 'git submodule update --init' on qtlocation.")
    } else {
        SUBDIRS += mapboxgl ../../3rdparty/mapbox-gl-native
        mapboxgl.depends = ../../3rdparty/mapbox-gl-native
    }
}
