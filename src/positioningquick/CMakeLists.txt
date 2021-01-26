# Generated from positioningquick.pro.

#####################################################################
## PositioningQuick Module:
#####################################################################

qt_internal_add_module(PositioningQuick
    GENERATE_METATYPES
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
        Qt::CorePrivate
        Qt::PositioningPrivate
        Qt::QmlPrivate
        Qt::QuickPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
        Qt::Qml
        Qt::Quick
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::PositioningPrivate
        Qt::QmlPrivate
        Qt::QuickPrivate
)

set_target_properties(PositioningQuick PROPERTIES
    QT_QML_MODULE_INSTALL_QMLTYPES TRUE
    QT_QML_MODULE_VERSION ${CMAKE_PROJECT_VERSION}
    QT_QML_MODULE_URI QtPositioning
    QT_QMLTYPES_FILENAME plugins.qmltypes
    QT_QML_MODULE_INSTALL_DIR "${INSTALL_QMLDIR}/QtPositioning"
)

qt6_qml_type_registration(PositioningQuick)
