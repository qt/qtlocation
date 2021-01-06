# Generated from serialnmea.pro.

#####################################################################
## QGeoPositionInfoSourceFactorySerialNmea Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactorySerialNmea
    OUTPUT_NAME qtposition_serialnmea
    TYPE position
    SOURCES
        qgeopositioninfosourcefactory_serialnmea.cpp qgeopositioninfosourcefactory_serialnmea.h
        qiopipe.cpp qiopipe_p.h
        qnmeasatelliteinfosource.cpp qnmeasatelliteinfosource_p.h
    PUBLIC_LIBRARIES
        Qt::CorePrivate
        Qt::PositioningPrivate
        Qt::SerialPort
)

#### Keys ignored in scope 1:.:.:serialnmea.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
