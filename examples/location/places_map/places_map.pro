TARGET = places_map
TEMPLATE = app

QT += quick qml network
SOURCES = main.cpp

RESOURCES += \
    places_map_resource.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/location/places_map
INSTALLS += target

