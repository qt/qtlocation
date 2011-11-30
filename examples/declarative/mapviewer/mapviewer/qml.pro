TEMPLATE = aux

files.files = info.json notions.json settings.json icon.png mapviewer.qml
files.path = /opt/mt/applications/mt-qt5location-mapviewer

components.path = $${files.path}/content/components
components.files = \
    content/components/Button.qml \
    content/components/Checkbox.qml \
    content/components/Fader.qml \
    content/components/Menu.qml \
    content/components/Optionbutton.qml \
    content/components/Slider.qml \
    content/components/TextWithLabel.qml \
    content/components/TitleBar.qml \
    content/components/Optionbutton.qml

style.path = $${files.path}/content/components/style
style.files = \
    content/components/style/ButtonStyle.qml \
    content/components/style/HMenuItemStyle.qml \
    content/components/style/VMenuItemStyle.qml

dialogs.path = $${files.path}/content/dialogs
dialogs.files = \
    content/dialogs/Dialog.qml \
    content/dialogs/Message.qml \
    content/dialogs/RouteDialog.qml

map.path = $${files.path}/content/map
map.files = \
    content/map/MapComponent.qml \
    content/map/Marker.qml \
    content/map/CircleItem.qml \
    content/map/RectangleItem.qml \
    content/map/PolylineItem.qml \
    content/map/PolygonItem.qml

resources.path =  $${files.path}/content/resources
resources.files = \
    content/resources/button_hovered.png \
    content/resources/button_pressed.png \
    content/resources/button.png \
    content/resources/button.sci \
    content/resources/catch.png \
    content/resources/checkbox_selected.png \
    content/resources/checkbox.png \
    content/resources/hmenuItem_hovered.png \
    content/resources/hmenuItem_pressed.png \
    content/resources/hmenuItem.png \
    content/resources/marker_hovered.png \
    content/resources/marker_selected.png \
    content/resources/marker.png \
    content/resources/node_selected.png \
    content/resources/node.png \
    content/resources/option_button_selected.png \
    content/resources/option_button.png \
    content/resources/quit_hovered.png \
    content/resources/quit_pressed.png \
    content/resources/quit.png \
    content/resources/scale.png \
    content/resources/scale_end.png \
    content/resources/titlebar.png \
    content/resources/titlebar.sci \
    content/resources/vmenuItem.png \
    content/resources/hmenuItem.sci \
    content/resources/vmenuItem.sci

INSTALLS += files components style dialogs map resources
