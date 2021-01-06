# Generated from positionpluginV1.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryTestV1 Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryTestV1
    OUTPUT_NAME qtposition_testpluginv1
    TYPE position
    DEFAULT_IF FALSE
    SOURCES
        plugin.cpp
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:positionpluginV1.pro:<TRUE>:
# OTHER_FILES = "plugin.json"
# PLUGIN_EXTENDS = "-"
