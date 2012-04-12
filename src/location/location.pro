load(qt_module)

TARGET = QtLocation
QPRO_PWD   = $$PWD

CONFIG += module

QT = core-private gui

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

load(qt_module_config)

HEADERS += qtlocationversion.h \
    qgeopositioninfosource_p.h

include(maps/maps.pri)
include(places/places.pri)

PUBLIC_HEADERS += \
                    qgeoaddress.h \
                    qgeoareamonitor.h \
                    qgeoshape.h \
                    qgeorectangle.h \
                    qgeocircle.h \
                    qgeocoordinate.h \
                    qgeolocation.h \
                    qgeopositioninfo.h \
                    qgeopositioninfosource.h \
                    qgeosatelliteinfo.h \
                    qgeosatelliteinfosource.h \
                    qnmeapositioninfosource.h \
                    qgeopositioninfosourcefactory.h \
                    qtlocation.h \
                    qlocationglobal.h

PRIVATE_HEADERS += \
                    qgeoaddress_p.h \
                    qgeoshape_p.h \
                    qgeorectangle_p.h \
                    qgeocircle_p.h \
                    qgeolocation_p.h \
                    qlocationutils_p.h \
                    qnmeapositioninfosource_p.h \
                    qgeoareamonitor_polling_p.h \
                    qgeocoordinate_p.h


HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
            qgeoaddress.cpp \
            qgeoareamonitor.cpp \
            qgeoshape.cpp \
            qgeorectangle.cpp \
            qgeocircle.cpp \
            qgeocoordinate.cpp \
            qgeolocation.cpp \
            qgeopositioninfo.cpp \
            qgeopositioninfosource.cpp \
            qgeosatelliteinfo.cpp \
            qgeosatelliteinfosource.cpp \
            qlocationutils.cpp \
            qnmeapositioninfosource.cpp \
            qgeoareamonitor_polling.cpp \
            qgeopositioninfosourcefactory.cpp \
            qtlocation.cpp \
