qt_internal_add_qml_module(locationlabsplugin
    URI Qt.labs.location
    VERSION ${PROJECT_VERSION}
    PLUGIN_TARGET locationlabsplugin
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL
    CLASS_NAME QtLocationLabsDeclarativeModule
    SOURCES
        locationlabs.cpp
        locationlabssingleton_p.h locationlabssingleton.cpp
    LIBRARIES
        Qt::LocationPrivate
)
