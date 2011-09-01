load(qt_module)

TARGET = qtgeoservices_nokia_places_jsondb
QT += location network script

load(qt_plugin)

DESTDIR = $$QT.location.plugins/geoservices
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"


HEADERS += \
            qgeoserviceproviderplugin_jsondb.h \
            qplacemanagerengine_jsondb.h \
            jsondbhandler.h \
            searchreply.h \
            detailsreply.h \
            reply.h \
            unsupportedreplies.h \
            macro.h \
            idreply.h

SOURCES += \
            qgeoserviceproviderplugin_jsondb.cpp \
            qplacemanagerengine_jsondb.cpp \
            jsondbhandler.cpp \
            searchreply.cpp \
            detailsreply.cpp \
            reply.cpp \
            idreply.cpp

INCLUDEPATH += $$QT.location.includes


DEFINES += QT_ADDON_JSONDB_LIB
QT += jsondb

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002BFCA
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep
    LIBS += -lefsrv
}

target.path += $$[QT_INSTALL_PLUGINS]/geoservices
INSTALLS += target
