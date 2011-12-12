TARGET = qml_location_mapviewer
TEMPLATE = app

QT += declarative network quick
SOURCES += qmlmapviewerwrapper.cpp

RESOURCES += \
    mapviewerwrapper.qrc

include(mapviewer.pri)

target.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/mapviewer

INSTALLS += target
