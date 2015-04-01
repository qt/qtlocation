TARGET = qml_location_mapviewer
TEMPLATE = app

QT += qml network quick positioning location
SOURCES += qmlmapviewerwrapper.cpp

RESOURCES += \
    mapviewerwrapper.qrc

qmlcontent.files += \
    mapviewer.qml

OTHER_FILES += $$qmlcontent.files

qmlcontentmap.files += \
    content/map/MapComponent.qml \
    content/map/Marker.qml \
    content/map/CircleItem.qml \
    content/map/RectangleItem.qml \
    content/map/PolylineItem.qml \
    content/map/PolygonItem.qml \
    content/map/ImageItem.qml \
    content/map/MiniMap.qml
OTHER_FILES += $$qmlcontentmap.files

qmlcontentdialogs.files += \
    content/dialogs/Message.qml \
    content/dialogs/RouteDialog.qml
OTHER_FILES += $$qmlcontentdialogs.files

include(../common/common.pri)

target.path = $$[QT_INSTALL_EXAMPLES]/location/mapviewer
additional.files = ../common
additional.path = $$[QT_INSTALL_EXAMPLES]/location/common
INSTALLS += target additional
