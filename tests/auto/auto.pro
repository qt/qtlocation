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
           qplacerating \
           qplacereview \
           qplacesearchrequest \
           qplacesupplier \
           qplacesearchresult \
           declarative \
           intervaltree \
           mapitemtree \
           sphere

contains(config_test_jsondb, yes) {
    SUBDIRS += qplacemanager_jsondb
}

!contains(QT_CONFIG,private-tests):SUBDIRS -= \
    sphere
