TEMPLATE = subdirs

mapviewer.file = qml.pro
!no_qmlwrapper:mapviewer.CONFIG += no_default_install no_default_target

mapviewerwrapper.file = mapviewerwrapper.pro
no_qmlwrapper:mapviewerwrapper.CONFIG += no_default_install no_default_target

SUBDIRS += mapviewerwrapper mapviewer
