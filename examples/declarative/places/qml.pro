TEMPLATE = aux

files.files = info.json notions.json settings.json icon.png
files.path = /opt/mt/applications/mt-qt5location-places

include(places.pri)

qmlcontent.path = $$files.path
qmlcontentdialogs.path = $$files.path/content/dialogs
qmlcontentmap.path = $$files.path/content/map
qmlcontentplaces.path = $$files.path/content/places
qmlcontentcomponents.path = $$files.path/content/components
qmlcontentcomponentsstyle.path = $$files.path/content/components/style

resources.files = content/resources/*
resources.path = $$files.path/content/resources

INSTALLS += \
    files \
    qmlcontent \
    qmlcontentdialogs \
    qmlcontentmap \
    qmlcontentplaces \
    qmlcontentcomponents \
    qmlcontentcomponentsstyle \
    resources
