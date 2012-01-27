TEMPLATE = subdirs

#Place unit tests
SUBDIRS += qplace \
           qplacecategory \
           qplacecontactdetail \
           qplacecontentrequest \
           qplacedetailsreply \
           qplaceeditorial \
           qplacemanager \
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
           positionplugin \
           positionplugintest \
           qgeocodingmanagerplugins \
           qgeoaddress \
           qgeoareamonitor \
           qgeoboundingbox \
           qgeoboundingcircle \
           qgeocodereply \
           qgeocodingmanager \
           qgeocoordinate \
           qgeolocation \
           qgeomaneuver \
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
           qnmeapositioninfosource \
           declarative_core \
           maptype

!mac: SUBDIRS += declarative_ui

!isEmpty(QT.jsondb.name):SUBDIRS += qplacemanager_jsondb
