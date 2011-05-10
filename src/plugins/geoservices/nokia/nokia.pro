TARGET = qtgeoservices_nokia
QT += location network

include($$QT_SOURCE_TREE/src/plugins/qpluginbase.pri)

DESTDIR = $$QT.location.plugins/geoservices
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"

HEADERS += \
            qgeocodexmlparser.h \
            qgeomappingmanagerengine_nokia.h \
            qgeomapreply_nokia.h \
            qgeoroutereply_nokia.h \
            qgeoroutexmlparser.h \
            qgeoroutingmanagerengine_nokia.h \
            qgeosearchmanagerengine_nokia.h \
            qgeosearchreply_nokia.h \
            qgeoserviceproviderplugin_nokia.h \
            marclanguagecodes.h \
            qgeotiledmapdata_nokia.h

SOURCES += \
            qgeocodexmlparser.cpp \
            qgeomappingmanagerengine_nokia.cpp \
            qgeomapreply_nokia.cpp \
            qgeoroutereply_nokia.cpp \
            qgeoroutexmlparser.cpp \
            qgeoroutingmanagerengine_nokia.cpp \
            qgeosearchmanagerengine_nokia.cpp \
            qgeosearchreply_nokia.cpp \
            qgeoserviceproviderplugin_nokia.cpp \
            qgeotiledmapdata_nokia.cpp

RESOURCES += resource.qrc

INCLUDEPATH += $$QT.location.includes

#INCLUDEPATH += $$SOURCE_DIR/src/location \
#                $$SOURCE_DIR/src/location/maps \
#                $$SOURCE_DIR/src/location/maps/tiled

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

