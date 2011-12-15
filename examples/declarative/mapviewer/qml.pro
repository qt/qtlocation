TEMPLATE = aux

files.files = info.json notions.json settings.json icon.png
files.path = /opt/mt/applications/mt-qt5location-mapviewer

include(mapviewer.pri)

qmlcontent.path = $$files.path
qmlcontentmap.path = $$files.path/content/map
qmlcontentdialogs.path = $$files.path/content/dialogs
qmlcomponents.path = $$files.path/imports/QtLocation/examples/components
qmlcomponentsstyle.path = $$qmlcomponents.path/style
qmldialogs.path = $$files.path/imports/QtLocation/examples/dialogs
qmldir.path = $$files.path/imports/QtLocation/examples
commonresources.path = $$files.path/resources

INSTALLS += \
    files \
    qmlcontent \
    qmlcontentmap \
    qmlcontentdialogs \
    qmlcomponents \
    qmlcomponentsstyle \
    qmldialogs \
    qmldir \
    commonresources
