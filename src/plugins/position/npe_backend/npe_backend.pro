load(qt_module)

TARGET = qtposition_npe_backend
QT += location gui

load(qt_plugin)

DESTDIR = $$QT.location.plugins/position
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"

INCLUDEPATH += $$QT.location.includes

DEFINES += NPE_BACKEND
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/include/mtlocationd
#unix {
#    CONFIG += link_pkgconfig
#    PKGCONFIG += mtlocationd
#}
LIBS += -lmtlocationd
SOURCES += qgeopositioninfosource_npe_backend.cpp \
           qgeosatelliteinfosource_npe_backend.cpp \
           qgeopositioninfosourcefactory_npe_backend.cpp
HEADERS += qgeopositioninfosource_npe_backend_p.h \
           qgeosatelliteinfosource_npe_backend_p.h \
           qgeopositioninfosourcefactory_npe_backend.h

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += \
    plugin.json
