load(qt_module)

TARGET = qtgeoservices_nokia
QT += location network

load(qt_plugin)

DESTDIR = $$QT.location.plugins/geoservices
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"

HEADERS += \
            qgeocodereply_nokia.h \
            qgeocodexmlparser.h \
            qgeocodingmanagerengine_nokia.h \
            qgeomappingmanagerengine_nokia.h \
            qgeomapreply_nokia.h \
            qgeoroutereply_nokia.h \
            qgeoroutexmlparser.h \
            qgeoroutingmanagerengine_nokia.h \
            qgeoserviceproviderplugin_nokia.h \
            marclanguagecodes.h \
            qgeotiledmapdata_nokia.h

SOURCES += \
            qgeocodereply_nokia.cpp \
            qgeocodexmlparser.cpp \
            qgeocodingmanagerengine_nokia.cpp \
            qgeomappingmanagerengine_nokia.cpp \
            qgeomapreply_nokia.cpp \
            qgeoroutereply_nokia.cpp \
            qgeoroutexmlparser.cpp \
            qgeoroutingmanagerengine_nokia.cpp \
            qgeoserviceproviderplugin_nokia.cpp \
            qgeotiledmapdata_nokia.cpp

include(places/places.pri)

RESOURCES += resource.qrc

INCLUDEPATH += $$QT.location.includes

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

