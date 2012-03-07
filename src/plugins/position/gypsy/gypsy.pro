load(qt_module)

TARGET = qtposition_gypsy
QT += location gui

load(qt_plugin)

DESTDIR = $$QT.location.plugins/position
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"

HEADERS += \
    qgeosatelliteinfosource_gypsy_p.h \
    qgeopositioninfosourcefactory_gypsy.h

SOURCES += \
    qgeosatelliteinfosource_gypsy.cpp \
    qgeopositioninfosourcefactory_gypsy.cpp

INCLUDEPATH += $$QT.location.includes

CONFIG += qdbus link_pkgconfig
DEFINES += GYPSY_AVAILABLE=1
PKGCONFIG += gypsy
QMAKE_PKGCONFIG_REQUIRES += gypsy

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += \
    plugin.json
