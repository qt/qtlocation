# Generated from positionplugin.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryTest Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryTest
    OUTPUT_NAME qtposition_testplugin
    PLUGIN_TYPE position
    DEFAULT_IF FALSE
    SOURCES
        plugin.cpp
    LIBRARIES
        Qt::Core
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:positionplugin.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
# PLUGIN_EXTENDS = "-"
