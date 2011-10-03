load(qt_module)

TARGET = QtLocation
QPRO_PWD   = $$PWD

CONFIG += module
MODULE_PRI = ../../modules/qt_location.pri

QT = core gui network

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += $$QT.location.private_includes/QtLocation

load(qt_module_config)

HEADERS += qtlocationversion.h

include(maps/maps.pri)
include(mapsgl/mapsgl.pri)
include(places/places.pri)

PUBLIC_HEADERS += \
                    qgeoaddress.h \
                    qgeoareamonitor.h \
                    qgeoboundingarea.h \
                    qgeoboundingbox.h \
                    qgeoboundingcircle.h \
                    qgeocoordinate.h \
                    qgeolocation.h \
                    qplace.h \
                    qgeopositioninfo.h \
                    qgeopositioninfosource.h \
                    qgeosatelliteinfo.h \
                    qgeosatelliteinfosource.h \
                    qnmeapositioninfosource.h \
                    qgeopositioninfosourcefactory.h \
                    qlatin1constant.h \
                    qmobilitypluginsearch.h \
                    qtlocation.h

PRIVATE_HEADERS += \
                    qgeoaddress_p.h \
                    qgeoboundingbox_p.h \
                    qgeoboundingcircle_p.h \
                    qgeolocation_p.h \
                    qplace_p.h \
                    qlocationutils_p.h \
                    qnmeapositioninfosource_p.h \
                    qgeoareamonitor_polling_p.h \
                    qgeocoordinate_p.h

symbian {
    PRIVATE_HEADERS += qgeopositioninfosource_s60_p.h \
                       qmlbackendao_s60_p.h \
                       qgeosatelliteinfosource_s60_p.h \
                       notificationcallback_s60_p.h \
                       notificationsatellitecallback_s60_p.h

    contains(lbt_enabled, yes) {
        PRIVATE_HEADERS += \
                       qgeoareamonitor_s60_p.h \
                       qmlbackendmonitorao_s60_p.h \
                       qmlbackendmonitorcreatetriggerao_s60_p.h \
                       qmlbackendmonitorinfo_s60_p.h \
                       qmlbackendtriggerchangeao_s60_p.h \
                       notificationmonitorcallback_s60_p.h
    }

    SOURCES += qgeopositioninfosource_s60.cpp \
               qgeosatelliteinfosource_s60.cpp \
               qmlbackendao_s60.cpp

    contains(lbt_enabled, yes) {
        SOURCES += \
               qgeoareamonitor_s60.cpp \
               qmlbackendmonitorao_s60.cpp \
               qmlbackendmonitorcreatetriggerao_s60.cpp \
               qmlbackendmonitorinfo_s60.cpp \
               qmlbackendtriggerchangeao_s60.cpp
    }
}


maemo6|meego {
    CONFIG += qdbus link_pkgconfig
    SOURCES += qgeopositioninfosource_maemo.cpp \
                qgeosatelliteinfosource_maemo.cpp \
                dbuscomm_maemo.cpp \
                dbusserver_maemo.cpp
    PRIVATE_HEADERS += qgeopositioninfosource_maemo_p.h \
                qgeosatelliteinfosource_maemo_p.h \
                dbuscomm_maemo_p.h \
                dbusserver_maemo_p.h
}


meego {
    contains (geoclue-master_enabled, yes) {
        message("Building location with GeoClue master support.")
        SOURCES += qgeopositioninfosource_geocluemaster.cpp
        PRIVATE_HEADERS += qgeopositioninfosource_geocluemaster_p.h
        DEFINES += GEOCLUE_MASTER_AVAILABLE=1
        PKGCONFIG += geoclue
        QMAKE_PKGCONFIG_REQUIRES += geoclue
    } else {
        message("Building location without GeoClue master support.")
    }
    contains (gypsy_enabled, yes) {
        message("Building location with Gypsy support.")
        SOURCES += qgeosatelliteinfosource_gypsy.cpp
        PRIVATE_HEADERS += qgeosatelliteinfosource_gypsy_p.h
        DEFINES += GYPSY_AVAILABLE=1
        PKGCONFIG += gypsy
        QMAKE_PKGCONFIG_REQUIRES += gypsy
    } else {
        message("Building location without Gypsy support.")
    }
    if (contains(gypsy_enabled, yes) || contains (geoclue-master_enabled, yes)) {
        CONFIG += qdbus link_pkgconfig
        PKGCONFIG += gconf-2.0 glib-2.0
        QMAKE_PKGCONFIG_REQUIRES += glib-2.0 gconf-2.0
    }
}

contains(config_test_jsondb, yes): contains(config_test_mtcore, yes) {
    DEFINES += NPE_BACKEND
    QT += jsondb-private
    SOURCES += qgeopositioninfosource_npe_backend.cpp
    PRIVATE_HEADERS += qgeopositioninfosource_npe_backend_p.h
    unix{
      CONFIG += link_pkgconfig
      PKGCONFIG += mtcore
    }
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
            qgeoaddress.cpp \
            qgeoareamonitor.cpp \
            qgeoboundingarea.cpp \
            qgeoboundingbox.cpp \
            qgeoboundingcircle.cpp \
            qgeocoordinate.cpp \
            qgeolocation.cpp \
            qplace.cpp \
            qgeopositioninfo.cpp \
            qgeopositioninfosource.cpp \
            qgeosatelliteinfo.cpp \
            qgeosatelliteinfosource.cpp \
            qlocationutils.cpp \
            qnmeapositioninfosource.cpp \
            qgeoareamonitor_polling.cpp \
            qgeopositioninfosourcefactory.cpp \
            qtlocation.cpp

symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC83

    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions \
                   $${EPOCROOT}epoc32/include/LBTHeaders \
                   $${EPOCROOT}epoc32/include/platform
    LIBS += -llbs
    LIBS += -lefsrv
    contains(lbt_enabled, yes) {
        LIBS += -llbt
    }

    QtLocationDeployment.sources = QtLocation.dll
    QtLocationDeployment.path = /sys/bin
    DEPLOYMENT += QtLocationDeployment
}

simulator {
    QT += gui
    SOURCES += qgeopositioninfosource_simulator.cpp \
                qlocationdata_simulator.cpp \
                qgeosatelliteinfosource_simulator.cpp \
                qlocationconnection_simulator.cpp
    HEADERS += qgeopositioninfosource_simulator_p.h \
                qlocationdata_simulator_p.h \
                qgeosatelliteinfosource_simulator_p.h \
                qlocationconnection_simulator_p.h
    INCLUDEPATH += ../mobilitysimulator
    qtAddLibrary(QtMobilitySimulator)
}

