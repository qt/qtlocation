# special case begin
qt_internal_add_qml_module(declarative_positioning
    URI "QtPositioning"
    VERSION "${CMAKE_PROJECT_VERSION}"
    CLASSNAME QtPositioningDeclarativeModule
    SKIP_TYPE_REGISTRATION
    SOURCES
        positioning.cpp
    PUBLIC_LIBRARIES
        Qt::QuickPrivate
        Qt::PositioningPrivate
        Qt::PositioningQuickPrivate
        Qt::QmlPrivate
        Qt::CorePrivate
)
# special case end
