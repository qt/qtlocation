TEMPLATE = subdirs

SUBDIRS += geotestplugin \
           qgeocodingmanagerplugins \
           qgeoaddress \
           qgeoboundingbox \
           qgeoboundingcircle \
           qgeocodereply \
           qgeocodingmanager \
           qgeocoordinate \
           qgeolocation \
           qgeomaneuver \
           qgeopositioninfo \
           qgeosatelliteinfo \
           qplace \
           qplacecategory \
           qplacecontentrequest \
           qplaceeditorial \
           qplacemanager \
           qplaceimage \
           qplacerequest \
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
