TARGET = QtPositioning
QT = core-private

QMAKE_DOCS = $$PWD/doc/qtpositioning.qdocconf
OTHER_FILES += doc/src/*.qdoc   # show .qdoc files in Qt Creator

PUBLIC_HEADERS += \
                    qgeoaddress.h \
                    qgeoareamonitorinfo.h \
                    qgeoareamonitorsource.h \
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
                    qpositioningglobal.h

PRIVATE_HEADERS += \
                    qgeoaddress_p.h \
                    qgeoshape_p.h \
                    qgeorectangle_p.h \
                    qgeocircle_p.h \
                    qgeolocation_p.h \
                    qlocationutils_p.h \
                    qnmeapositioninfosource_p.h \
                    qgeocoordinate_p.h \
                    qgeopositioninfosource_p.h \
                    qdeclarativegeoaddress_p.h \
                    qdeclarativegeolocation_p.h

SOURCES += \
            qgeoaddress.cpp \
            qgeoareamonitorsource.cpp \
            qgeoareamonitorinfo.cpp \
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
            qgeopositioninfosourcefactory.cpp \
            qdeclarativegeoaddress.cpp \
            qdeclarativegeolocation.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
