# Generated from winrt.pro.

#####################################################################
## QGeoPositionInfoSourceFactoryWinRT Plugin:
#####################################################################

qt_internal_add_plugin(QGeoPositionInfoSourceFactoryWinRT
    OUTPUT_NAME qtposition_winrt
    TYPE position
    SOURCES
        qgeopositioninfosource_winrt.cpp qgeopositioninfosource_winrt_p.h
        qgeopositioninfosourcefactory_winrt.cpp qgeopositioninfosourcefactory_winrt.h
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::CorePrivate
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:winrt.pro:<TRUE>:
# OTHER_FILES = "plugin.json"

## Scopes:
#####################################################################

qt_internal_extend_target(QGeoPositionInfoSourceFactoryWinRT CONDITION MSVC AND NOT WINRT
    PUBLIC_LIBRARIES
        runtimeobject.lib
)
