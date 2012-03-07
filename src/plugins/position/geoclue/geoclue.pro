load(qt_module)

TARGET = qtposition_geoclue
QT += location gui

load(qt_plugin)

DESTDIR = $$QT.location.plugins/position
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"

HEADERS += \
    qgeopositioninfosource_geocluemaster_p.h \
    qgeopositioninfosourcefactory_geoclue.h

SOURCES += \
    qgeopositioninfosource_geocluemaster_p.h \
    qgeopositioninfosourcefactory_geoclue.cpp

INCLUDEPATH += $$QT.location.includes

CONFIG += qdbus link_pkgconfig
DEFINES += GEOCLUE_MASTER_AVAILABLE=1
PKGCONFIG += geoclue
QMAKE_PKGCONFIG_REQUIRES += geoclue

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += \
    plugin.json
