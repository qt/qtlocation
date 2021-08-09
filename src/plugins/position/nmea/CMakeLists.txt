# Generated from nmea.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryNmea Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryNmeaPlugin
    OUTPUT_NAME qtposition_nmea
    CLASS_NAME QGeoPositionInfoSourceFactoryNmea
    PLUGIN_TYPE position
    SOURCES
        qgeopositioninfosourcefactory_nmea.cpp qgeopositioninfosourcefactory_nmea.h
        qiopipe.cpp qiopipe_p.h
    LIBRARIES
        Qt::CorePrivate
        Qt::Positioning
        Qt::SerialPort
        Qt::Network
)

#### Keys ignored in scope 1:.:.:serialnmea.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
