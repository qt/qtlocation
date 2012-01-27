load(qt_module)

TARGET = qtgeoservices_nokia_places_jsondb
QT += location gui

load(qt_plugin)

DESTDIR = $$QT.location.plugins/geoservices
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"


HEADERS += \
            qgeoserviceproviderplugin_jsondb.h \
            qplacemanagerengine_jsondb.h \
            jsonconverter.h \
            matchreply.h \
            searchreply.h \
            detailsreply.h \
            reply.h \
            unsupportedreplies.h \
            macro.h \
            idreply.h \
            icon.h

SOURCES += \
            qgeoserviceproviderplugin_jsondb.cpp \
            qplacemanagerengine_jsondb.cpp \
            jsonconverter.cpp \
            matchreply.cpp \
            searchreply.cpp \
            detailsreply.cpp \
            reply.cpp \
            idreply.cpp \
            icon.cpp

INCLUDEPATH += $$QT.location.includes


DEFINES += QT_ADDON_JSONDB_LIB
QT += jsondb

target.path += $$[QT_INSTALL_PLUGINS]/geoservices
INSTALLS += target
