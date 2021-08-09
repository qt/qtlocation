# Generated from positionpoll.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryPoll Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryPollPlugin
    OUTPUT_NAME qtposition_positionpoll
    CLASS_NAME QGeoPositionInfoSourceFactoryPoll
    PLUGIN_TYPE position
    SOURCES
        positionpollfactory.cpp positionpollfactory.h
        qgeoareamonitor_polling.cpp qgeoareamonitor_polling.h
    LIBRARIES
        Qt::Core
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:positionpoll.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
