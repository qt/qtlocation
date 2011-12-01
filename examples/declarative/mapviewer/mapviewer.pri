
qmlcontent.files += \
    mapviewer.qml
OTHER_FILES += $$qmlcontent.files

qmlcontentmap.files += \
    content/map/MapComponent.qml \
    content/map/Marker.qml \
    content/map/CircleItem.qml \
    content/map/RectangleItem.qml \
    content/map/PolylineItem.qml \
    content/map/PolygonItem.qml
OTHER_FILES += $$qmlcontentmap.files

qmlcontentdialogs.files += \
    content/dialogs/Message.qml \
    content/dialogs/RouteDialog.qml
OTHER_FILES += $$qmlcontentdialogs.files

include(../common/common.pri)
