TARGET = qtposition_geoclue
QT += location gui

PLUGIN_TYPE = position
load(qt_plugin)

HEADERS += \
    qgeopositioninfosource_geocluemaster_p.h \
    qgeopositioninfosourcefactory_geoclue.h

SOURCES += \
    qgeopositioninfosource_geocluemaster.cpp \
    qgeopositioninfosourcefactory_geoclue.cpp

INCLUDEPATH += $$QT.location.includes

CONFIG += link_pkgconfig
PKGCONFIG += geoclue gconf-2.0

OTHER_FILES += \
    plugin.json
