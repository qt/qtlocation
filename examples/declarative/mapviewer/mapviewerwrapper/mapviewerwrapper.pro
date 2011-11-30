TARGET = qml_location_mapviewer
TEMPLATE=app

QT += declarative network
SOURCES += qmlmapviewerwrapper.cpp

RESOURCES += \
    mapviewerwrapper.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/mapviewer

INSTALLS += target

OTHER_FILES += \
    ../mapviewer/mapviewer.qml \
    ../mapviewer/content/dialogs/Dialog.qml \
    ../mapviewer/content/dialogs/Message.qml \
    ../mapviewer/content/dialogs/RouteDialog.qml \
    ../mapviewer/content/map/MapComponent.qml \
    ../mapviewer/content/map/Marker.qml \
    ../mapviewer/content/map/PolylineItem.qml \
    ../mapviewer/content/map/RectangleItem.qml \
    ../mapviewer/content/map/CircleItem.qml \
    ../mapviewer/content/map/PolygonItem.qml \
    ../mapviewer/content/components/TextWithLabel.qml \
    ../mapviewer/content/components/Button.qml \
    ../mapviewer/content/components/Checkbox.qml \
    ../mapviewer/content/components/Fader.qml \
    ../mapviewer/content/components/Optionbutton.qml \
    ../mapviewer/content/components/Slider.qml \
    ../mapviewer/content/components/TitleBar.qml \
    ../mapviewer/content/components/Menu.qml \
    ../mapviewer/content/components/style/ButtonStyle.qml \
    ../mapviewer/content/components/style/HMenuItemStyle.qml \
    ../mapviewer/content/components/style/VMenuItemStyle.qml
