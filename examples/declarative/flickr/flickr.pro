TEMPLATE=app
TARGET = qml_location_flickr

QT += declarative network location
SOURCES += qmllocationflickr.cpp

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    flickr.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/flickr
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.qml flickrcommon flickrmobile
sources.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/flickr

INSTALLS += target sources
