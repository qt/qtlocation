# special case begin
qt_internal_add_qml_module(declarative_positioning
    URI "QtPositioning"
    VERSION "${CMAKE_PROJECT_VERSION}"
    CLASSNAME QtPositioningDeclarativeModule
    SKIP_TYPE_REGISTRATION
    PLUGIN_OPTIONAL
    SOURCES
        positioning.cpp
    LIBRARIES
        Qt::PositioningQuickPrivate
    PUBLIC_LIBRARIES
        Qt::Quick
        Qt::Positioning
        Qt::PositioningQuick
        Qt::Qml
        Qt::Core
)
# special case end
