TEMPLATE = subdirs

SUBDIRS = nokia osm mapbox esri itemsoverlay

qtConfig(c++14):qtConfig(opengl):!win32 {
    !exists(../../3rdparty/mapbox-gl-native/CMakeLists.txt) {
        warning("Submodule mapbox-gl-native does not exist. Run 'git submodule update --init' on qtlocation.")
    } else {
        SUBDIRS += mapboxgl ../../3rdparty/mapbox-gl-native
        mapboxgl.depends = ../../3rdparty/mapbox-gl-native
    }
}
