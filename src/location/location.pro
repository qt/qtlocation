TARGET = QtLocation
QT = core-private


QMAKE_DOCS = $$PWD/doc/qtlocation.qdocconf
OTHER_FILES += doc/src/*.qdoc   # show .qdoc files in Qt Creator

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
                    qlocation.h \
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
                    qgeocoordinate_p.h \
                    qgeopositioninfosource_p.h

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
            qlocation.cpp

qtHaveModule(3d): include(maps/maps.pri)
include(places/places.pri)

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
