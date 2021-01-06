# Generated from simulator.pro.

#####################################################################
## QGeoPositionInfoSourceFactorySimulator Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactorySimulator
    OUTPUT_NAME qtposition_simulator
    TYPE position
    SOURCES
        qgeopositioninfosource_simulator.cpp qgeopositioninfosource_simulator_p.h
        qgeopositioninfosourcefactory_simulator.cpp qgeopositioninfosourcefactory_simulator.h
        qgeosatelliteinfosource_simulator.cpp qgeosatelliteinfosource_simulator_p.h
        qlocationconnection_simulator.cpp qlocationconnection_simulator_p.h
    DEFINES
        QT_SIMULATOR
    INCLUDE_DIRECTORIES
        ../../../positioning
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Network
        Qt::Positioning
        simulator
)

#### Keys ignored in scope 1:.:.:simulator.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
