# special case begin
qt_internal_add_qml_module(declarative_positioning
    URI "QtPositioning"
    VERSION "${CMAKE_PROJECT_VERSION}"
    CLASSNAME QtPositioningDeclarativeModule
    SKIP_TYPE_REGISTRATION
    #GENERATE_QMLTYPES
    INSTALL_QMLTYPES
    SOURCES
        positioning.cpp
        locationsingleton.cpp
        locationsingleton.h
    PUBLIC_LIBRARIES
        Qt::QuickPrivate
        Qt::PositioningPrivate
        Qt::PositioningQuickPrivate
        Qt::QmlPrivate
        Qt::CorePrivate
)
# special case end
