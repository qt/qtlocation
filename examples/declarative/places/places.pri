
qmlcontent.files += \
    places.qml
OTHER_FILES += $$qmlcontent.files

qmlcontentplaces.files += \
    content/places/PlacesUtils.js \
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
    content/places/PlaceImages.qml \
    content/places/MapComponent.qml \
    content/places/OptionsDialog.qml \
    content/places/CategoryView.qml

OTHER_FILES += $$qmlcontentplaces.files

include(../common/common.pri)
