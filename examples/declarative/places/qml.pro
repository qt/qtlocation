TEMPLATE = aux

files.files = info.json notions.json settings.json icon.png
files.path = /opt/mt/applications/mt-qt5location-places

include(places.pri)

qmlcontent.path = $$files.path
qmlcontentplaces.path = $$files.path/content/places
qmlcomponents.path = $$files.path/imports/QtLocation/examples/components
qmlcomponentsstyle.path = $$qmlcomponents.path/style
qmldialogs.path = $$files.path/imports/QtLocation/examples/dialogs
qmldir.path = $$files.path/imports/QtLocation/examples
commonresources.path = $$files.path/resources

INSTALLS += \
    files \
    qmlcontent \
    qmlcontentplaces \
    qmlcomponents \
    qmlcomponentsstyle \
    qmldialogs \
    qmldir \
    commonresources
