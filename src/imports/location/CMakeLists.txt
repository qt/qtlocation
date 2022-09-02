qt_internal_add_qml_module(declarative_location
    URI QtLocation
    VERSION ${PROJECT_VERSION}
    PLUGIN_TARGET declarative_location
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL
    CLASS_NAME QtLocationDeclarativeModule
    SOURCES
        location.h location.cpp
    LIBRARIES
        Qt::LocationPrivate
        Qt::Quick
)
