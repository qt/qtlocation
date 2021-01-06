# Generated from positioningquick.pro.

#####################################################################
## PositioningQuick Module:
#####################################################################

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

qt_add_module(PositioningQuick
    SOURCES
        ${SOURCES}
        ${HEADERS}
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
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
