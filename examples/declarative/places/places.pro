TEMPLATE = subdirs

places.file = qml.pro
!no_qmlwrapper:places.CONFIG += no_default_install no_default_target

placeswrapper.file = placeswrapper.pro
no_qmlwrapper:placeswrapper.CONFIG += no_default_install no_default_target

SUBDIRS += placeswrapper places
