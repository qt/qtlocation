TEMPLATE = subdirs

#Place unit tests
SUBDIRS += qplace \
           qplacecategory \
           qplacecontentrequest \
           qplaceeditorial \
           qplacemanager \
           qplacemanager_nokia \
           qplaceimage \
           qplaceratings \
           qplacereview \
           qplacesearchrequest \
           qplacesupplier \
           qplacesearchresult \
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
           qgeosatelliteinfo \
           qgeosatelliteinfosource \
           qnmeapositioninfosource \
           declarative_core \
           maptype

!mac: SUBDIRS += declarative_ui

contains(config_test_jsondb, yes) {
    SUBDIRS += qplacemanager_jsondb
}
