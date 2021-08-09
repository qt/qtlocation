# Generated from src.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryAndroid Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryAndroidPlugin
    OUTPUT_NAME qtposition_android
    CLASS_NAME QGeoPositionInfoSourceFactoryAndroid
    PLUGIN_TYPE position
    SOURCES
        jnipositioning.cpp jnipositioning.h
        positionfactory_android.cpp positionfactory_android.h
        qgeopositioninfosource_android.cpp qgeopositioninfosource_android_p.h
        qgeosatelliteinfosource_android.cpp qgeosatelliteinfosource_android_p.h
    LIBRARIES
        Qt::Core
        Qt::CorePrivate
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:src.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
