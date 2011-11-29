
qmlcontent.files = \
    places.qml
OTHER_FILES += $$qmlcontent.files

qmlcontentdialogs.files = \
    content/dialogs/Dialog.qml \
    content/dialogs/Message.qml \
    content/dialogs/RouteDialog.qml
OTHER_FILES += $$qmlcontentdialogs.files

qmlcontentmap.files = \
    content/map/MapComponent.qml \
    content/map/MapBaseComponent.qml \
    content/map/MosPlugin.qml \
    content/map/RestPlugin.qml \
    content/map/Marker.qml
OTHER_FILES += $$qmlcontentmap.files

qmlcontentplaces.files = \
    content/places/Group.qml \
    content/places/SearchBox.qml \
    content/places/CategoryDelegate.qml \
    content/places/SearchResultDelegate.qml \
    content/places/PlaceDelegate.qml \
    content/places/RatingView.qml \
    content/places/SearchResultView.qml \
    content/places/PlaceDialog.qml \
    content/places/CategoryDialog.qml \
    content/places/PlaceEditorials.qml \
    content/places/EditorialDelegate.qml \
    content/places/EditorialPage.qml \
    content/places/PlaceReviews.qml \
    content/places/ReviewDelegate.qml \
    content/places/ReviewPage.qml \
    content/places/PlaceImages.qml
OTHER_FILES += $$qmlcontentplaces.files

qmlcontentcomponents.files = \
    content/components/TextWithLabel.qml \
    content/components/Button.qml \
    content/components/Checkbox.qml \
    content/components/Fader.qml \
    content/components/Optionbutton.qml \
    content/components/Slider.qml \
    content/components/TitleBar.qml \
    content/components/ButtonRow.qml \
    content/components/Menu.qml \
    content/components/IconButton.qml \
    content/components/BusyIndicator.qml
OTHER_FILES += $$qmlcontentcomponents.files

qmlcontentcomponentsstyle.files = \
    content/components/style/Style.qml \
    content/components/style/ButtonStyle.qml \
    content/components/style/HMenuItemStyle.qml \
    content/components/style/VMenuItemStyle.qml
OTHER_FILES += $$qmlcontentcomponentsstyle.files
