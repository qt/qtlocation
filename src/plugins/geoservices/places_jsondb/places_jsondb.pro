TARGET = qtgeoservices_places_jsondb
QT += location gui network

PLUGIN_TYPE = geoservices
load(qt_plugin)

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
            iconhandler.h \
            initreply.h \
            traverser.h

SOURCES += \
            qgeoserviceproviderplugin_jsondb.cpp \
            qplacemanagerengine_jsondb.cpp \
            jsondb.cpp \
            matchreply.cpp \
            searchreply.cpp \
            detailsreply.cpp \
            idreply.cpp \
            icon.cpp \
            iconhandler.cpp \
            initreply.cpp \
            traverser.cpp

INCLUDEPATH += $$QT.location.includes

QT += jsondb

OTHER_FILES += \
    places_jsondb_plugin.json
