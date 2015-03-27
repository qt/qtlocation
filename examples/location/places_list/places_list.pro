TARGET = places_list
TEMPLATE = app

QT += quick qml network positioning location
SOURCES = main.cpp

RESOURCES += \
    resources.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/location/places_list
INSTALLS += target

