TEMPLATE = subdirs

#Place unit tests
SUBDIRS += qplace \
           qplaceattribute \
           qplacecategory \
           qplacecontactdetail \
           qplacecontentrequest \
           qplacedetailsreply \
           qplaceeditorial \
           qplacemanager \
           qplacemanager_unsupported \
           qplacemanager_nokia \
           qplacematchreply \
           qplacematchrequest \
           qplaceimage \
           qplaceratings \
           qplacereply \
           qplacereview \
           qplacesearchrequest \
           qplacesupplier \
           qplacesearchresult \
           qplacesearchreply \
           qplacesearchsuggestionreply \
           qplaceuser \
           qmlinterface

SUBDIRS += geotestplugin \
           placesplugin_unsupported \
           positionplugin \
           positionplugintest \
           qgeocodingmanagerplugins \
           qgeoaddress \
           qgeoareamonitor \
           qgeoboundingarea \
           qgeoboundingbox \
           qgeoboundingcircle \
           qgeocameradata \
           qgeocameracapabilities \
           qgeocameratiles \
           qgeocodereply \
           qgeocodingmanager \
           qgeocoordinate \
           qgeolocation \
           qgeomaneuver \
           qgeomapgeometry \
           qgeopositioninfo \
           qgeopositioninfosource \
           qgeoroute \
           qgeoroutereply \
           qgeorouterequest \
           qgeoroutesegment \
           qgeoroutingmanager \
           qgeoroutingmanagerplugins \
           qgeosatelliteinfo \
           qgeosatelliteinfosource \
           qgeotilespec \
           qnmeapositioninfosource \
           declarative_core \
           qgeoroutexmlparser \
           maptype

!mac: SUBDIRS += declarative_ui

!isEmpty(QT.jsondb.name):SUBDIRS += qplacemanager_jsondb
