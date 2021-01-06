# Generated from geoclue.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryGeoclue Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryGeoclue
    OUTPUT_NAME qtposition_geoclue
    TYPE position
    SOURCES
        geocluetypes.cpp geocluetypes.h
        qgeocluemaster.cpp qgeocluemaster.h
        qgeopositioninfosource_geocluemaster.cpp qgeopositioninfosource_geocluemaster.h
        qgeopositioninfosourcefactory_geoclue.cpp qgeopositioninfosourcefactory_geoclue.h
        qgeosatelliteinfosource_geocluemaster.cpp qgeosatelliteinfosource_geocluemaster.h
    DBUS_INTERFACE_SOURCES
        org.freedesktop.Geoclue.xml
        org.freedesktop.Geoclue.Master.xml
        org.freedesktop.Geoclue.MasterClient.xml
        org.freedesktop.Geoclue.Position.xml
        org.freedesktop.Geoclue.Satellite.xml
        org.freedesktop.Geoclue.Velocity.xml
    DBUS_INTERFACE_FLAGS
        -N -i geocluetypes.h # special case
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_BINARY_DIR}
        core.location.includes
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::DBus
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:geoclue.pro:<TRUE>:
# OTHER_FILES = "$$DBUS_INTERFACES" "plugin.json"
