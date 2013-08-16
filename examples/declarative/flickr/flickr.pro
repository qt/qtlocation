TEMPLATE=app
TARGET = qml_position_flickr

QT += qml quick network positioning
SOURCES += qmllocationflickr.cpp

RESOURCES += \
    flickr.qrc

OTHER_FILES += flickr.qml

target.path = $$[QT_INSTALL_EXAMPLES]/qtpositioning/declarative/flickr
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.qml flickrcommon flickrmobile
sources.path = $$[QT_INSTALL_EXAMPLES]/qtpositioning/declarative/flickr

INSTALLS += target sources
