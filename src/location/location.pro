load(qt_module)

TARGET = QtLocation
QPRO_PWD   = $$PWD

CONFIG += module
MODULE_PRI = ../../modules/qt_location.pri

QT = core core-private gui network

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += $$QT.location.private_includes/QtLocation

load(qt_module_config)

HEADERS += qtlocationversion.h \
    qgeopositioninfosource_p.h

include(maps/maps.pri)
include(places/places.pri)

PUBLIC_HEADERS += \
                    qgeoaddress.h \
                    qgeoareamonitor.h \
                    qgeoboundingarea.h \
                    qgeoboundingbox.h \
                    qgeoboundingcircle.h \
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
                    qgeoboundingarea_p.h \
                    qgeoboundingbox_p.h \
                    qgeoboundingcircle_p.h \
                    qgeolocation_p.h \
                    qlocationutils_p.h \
                    qnmeapositioninfosource_p.h \
                    qgeoareamonitor_polling_p.h \
                    qgeocoordinate_p.h


HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
            qgeoaddress.cpp \
            qgeoareamonitor.cpp \
            qgeoboundingarea.cpp \
            qgeoboundingbox.cpp \
            qgeoboundingcircle.cpp \
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
            qtlocation.cpp

