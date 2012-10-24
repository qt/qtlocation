TARGET = qtposition_npe_backend
QT += location gui

PLUGIN_TYPE = position
load(qt_plugin)

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

OTHER_FILES += \
    plugin.json
