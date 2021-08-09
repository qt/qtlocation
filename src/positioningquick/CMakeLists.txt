# Generated from positioningquick.pro.

#####################################################################
## PositioningQuick Module:
#####################################################################

qt_internal_add_qml_module(PositioningQuick
    URI QtPositioning
    VERSION ${CMAKE_PROJECT_VERSION}
    PLUGIN_TARGET positioningquickplugin
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL
    CLASS_NAME QtPositioningDeclarativeModule
    SOURCES
        qdeclarativegeoaddress_p.h qdeclarativegeoaddress.cpp
        qdeclarativegeolocation_p.h qdeclarativegeolocation.cpp
        qdeclarativepluginparameter_p.h qdeclarativepluginparameter.cpp
        qdeclarativeposition_p.h qdeclarativeposition.cpp
        qdeclarativepositionsource_p.h qdeclarativepositionsource.cpp
        qquickgeocoordinateanimation_p.h qquickgeocoordinateanimation.cpp
        locationsingleton_p.h locationsingleton.cpp
        qquickgeocoordinateanimation_p_p.h
        qpositioningquickglobal.h
        qpositioningquickglobal_p.h
        qpositioningquickmodule_p.h
    LIBRARIES
        Qt::PositioningPrivate
        Qt::QuickPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
        Qt::Qml
        Qt::Quick
    PRIVATE_MODULE_INTERFACE
        Qt::PositioningPrivate
        Qt::QuickPrivate
)

qt_internal_extend_target(positioningquickplugin
    SOURCES
        positioningplugin.cpp
    LIBRARIES
        Qt::PositioningQuickPrivate
        Qt::Quick
)

