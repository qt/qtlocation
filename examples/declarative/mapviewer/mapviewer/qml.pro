TEMPLATE = aux

files.files = info.json notions.json settings.json icon.png
#files.files = info.json notions.json settings.json mapviewer.qml content/HomePage.qml icon.png
files.path = /opt/mt/applications/mt-qt5location-mapviewer
INSTALLS = files

OTHER_FILES += \
    content/dialogs/Dialog.qml \
    content/dialogs/Message.qml \
    content/dialogs/RouteDialog.qml \
    content/map/MapComponent.qml \
    content/map/MapBaseComponent.qml \
    content/map/MosPlugin.qml \
    content/map/RestPlugin.qml \
    content/map/Marker.qml \
    content/components/TextWithLabel.qml \
    content/components/Button.qml \
    content/components/Checkbox.qml \
    content/components/Fader.qml \
    content/components/Optionbutton.qml \
    content/components/Slider.qml \
    content/components/TitleBar.qml \
    content/components/ButtonRow.qml \
    content/components/Menu.qml \
    content/components/style/Style.qml \
    content/components/style/ButtonStyle.qml \
    content/components/style/HMenuItemStyle.qml \
    content/components/style/VMenuItemStyle.qml

