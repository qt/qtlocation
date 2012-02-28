load(qt_module)

TARGET = qtgeoservices_nokia_places_jsondb
QT += location gui

load(qt_plugin)

DESTDIR = $$QT.location.plugins/geoservices
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"


HEADERS += \
            qgeoserviceproviderplugin_jsondb.h \
            qplacemanagerengine_jsondb.h \
            jsondb.h \
            matchreply.h \
            searchreply.h \
            detailsreply.h \
            unsupportedreplies.h \
            macro.h \
            idreply.h \
            icon.h \
            initreply.h

SOURCES += \
            qgeoserviceproviderplugin_jsondb.cpp \
            qplacemanagerengine_jsondb.cpp \
            jsondb.cpp \
            matchreply.cpp \
            searchreply.cpp \
            detailsreply.cpp \
            idreply.cpp \
            icon.cpp \
            initreply.cpp

INCLUDEPATH += $$QT.location.includes

QT += jsondb

target.path += $$[QT_INSTALL_PLUGINS]/geoservices
INSTALLS += target

OTHER_FILES += \
    nokia_plugin_jsondb.json
