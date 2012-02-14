
INCLUDEPATH += maps

QT += network 3d

PUBLIC_HEADERS += \
                    maps/qgeocodereply.h \
                    maps/qgeocodingmanagerengine.h \
                    maps/qgeocodingmanager.h \
                    maps/qgeomaneuver.h \
                    maps/qgeomappingmanagerengine.h \
                    maps/qgeomappingmanager.h \
                    maps/qgeomaptype.h \
                    maps/qgeoroute.h \
                    maps/qgeoroutereply.h \
                    maps/qgeorouterequest.h \
                    maps/qgeoroutesegment.h \
                    maps/qgeoroutingmanagerengine.h \
                    maps/qgeoroutingmanager.h \
                    maps/qgeoserviceproviderfactory.h \
                    maps/qgeoserviceprovider.h \
                    maps/qgeotiledmapreply.h \
                    maps/qgeotilespec.h

PRIVATE_HEADERS += \
                    maps/qdoublevector2d_p.h \
                    maps/qdoublevector3d_p.h \
                    maps/qgeocameradata_p.h \
                    maps/qgeocameratiles_p.h \
                    maps/qgeocodereply_p.h \
                    maps/qgeocodingmanagerengine_p.h \
                    maps/qgeocodingmanager_p.h \
                    maps/qgeocoordinateinterpolator_p.h \
                    maps/qgeomaneuver_p.h \
                    maps/qgeomapcontroller_p.h \
                    maps/qgeomapgeometry_p.h \
                    maps/qgeomapimages_p.h \
                    maps/qgeomap_p.h \
                    maps/qgeomap_p_p.h \
                    maps/qgeomappingmanagerengine_p.h \
                    maps/qgeomappingmanager_p.h \
                    maps/qgeomaptype_p.h \
                    maps/qgeoprojection_p.h \
                    maps/qgeoroute_p.h \
                    maps/qgeoroutereply_p.h \
                    maps/qgeorouterequest_p.h \
                    maps/qgeoroutesegment_p.h \
                    maps/qgeoroutingmanagerengine_p.h \
                    maps/qgeoroutingmanager_p.h \
                    maps/qgeoserviceprovider_p.h \
                    maps/qgeotilecache_p.h \
                    maps/qgeotiledmapreply_p.h \
                    maps/qgeotilespec_p.h

SOURCES += \
            maps/qdoublevector2d.cpp \
            maps/qdoublevector3d.cpp \
            maps/qgeocameradata.cpp \
            maps/qgeocameratiles.cpp \
            maps/qgeocodereply.cpp \
            maps/qgeocodingmanager.cpp \
            maps/qgeocodingmanagerengine.cpp \
            maps/qgeocoordinateinterpolator.cpp \
            maps/qgeomaneuver.cpp \
            maps/qgeomapcontroller.cpp \
            maps/qgeomapgeometry.cpp \
            maps/qgeomapimages.cpp \
            maps/qgeomap.cpp \
            maps/qgeomappingmanager.cpp \
            maps/qgeomappingmanagerengine.cpp \
            maps/qgeomaptype.cpp \
            maps/qgeoprojection.cpp \
            maps/qgeoroute.cpp \
            maps/qgeoroutereply.cpp \
            maps/qgeorouterequest.cpp \
            maps/qgeoroutesegment.cpp \
            maps/qgeoroutingmanager.cpp \
            maps/qgeoroutingmanagerengine.cpp \
            maps/qgeoserviceprovider.cpp \
            maps/qgeoserviceproviderfactory.cpp \
            maps/qgeotilecache.cpp \
            maps/qgeotiledmapreply.cpp \
            maps/qgeotilespec.cpp
