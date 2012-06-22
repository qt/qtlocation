load(qt_build_config)

TARGET = qtposition_geoclue
QT += location gui

load(qt_plugin)

DESTDIR = $$QT.location.plugins/position

HEADERS += \
    qgeopositioninfosource_geocluemaster_p.h \
    qgeopositioninfosourcefactory_geoclue.h

SOURCES += \
    qgeopositioninfosource_geocluemaster.cpp \
    qgeopositioninfosourcefactory_geoclue.cpp

INCLUDEPATH += $$QT.location.includes

CONFIG += link_pkgconfig
PKGCONFIG += geoclue gconf-2.0

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += \
    plugin.json
