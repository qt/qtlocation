TEMPLATE = subdirs

#Place unit tests
SUBDIRS += qplace \
           qplaceattribute \
           qplacecategory \
           qplacecontactdetail \
           qplacecontentrequest \
           qplacedetailsreply \
           qplaceeditorial \
           qplacematchreply \
           qplacematchrequest \
           qplaceimage \
           qplaceratings \
           qplaceresult \
           qplacereply \
           qplacereview \
           qplacesearchrequest \
           qplacesupplier \
           qplacesearchresult \
           qplacesearchreply \
           qplacesearchsuggestionreply \
           qplaceuser \
           qmlinterface \
           cmake

qtHaveModule(3d) {
    SUBDIRS += qplacemanager \
            qplacemanager_nokia \
            qplacemanager_unsupported
}

SUBDIRS += \
           positionplugin \
           positionplugintest \
           qgeoaddress \
           qgeoareamonitor \
           qgeoshape \
           qgeorectangle \
           qgeocircle \
           qgeocoordinate \
           qgeolocation \
           qgeopositioninfo \
           qgeopositioninfosource \
           qgeosatelliteinfo \
           qgeosatelliteinfosource \
           qnmeapositioninfosource \
           declarative_core \
           doublevectors

!mac: SUBDIRS += declarative_ui

qtHaveModule(3d) {
    SUBDIRS += geotestplugin \
           placesplugin_unsupported \
           qgeocodingmanagerplugins \
           qgeocameracapabilities\
           qgeocameradata \
           qgeocodereply \
           qgeocodingmanager \
           qgeomaneuver \
           qgeomapscene \
           qgeoroute \
           qgeoroutereply \
           qgeorouterequest \
           qgeoroutesegment \
           qgeoroutingmanager \
           qgeoroutingmanagerplugins \
           qgeotilespec \
           qgeoroutexmlparser \
           qgeomapcontroller \
           maptype \
           nokia_services \
           qgeocameratiles
}

qtHaveModule(jsondb): SUBDIRS += qplacemanager_jsondb
