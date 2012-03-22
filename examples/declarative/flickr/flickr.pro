TEMPLATE=app
TARGET = qml_location_flickr

QT += qml quick network location
SOURCES += qmllocationflickr.cpp

RESOURCES += \
    flickr.qrc

OTHER_FILES += flickr.qml

target.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/flickr
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.qml flickrcommon flickrmobile
sources.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/flickr

INSTALLS += target sources
