TEMPLATE = subdirs

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
           qplace \
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
           declarative \
           qmlinterface \
           maptype

contains(config_test_jsondb, yes) {
    SUBDIRS += qplacemanager_jsondb
}
