TEMPLATE = subdirs

SUBDIRS += geotestplugin \
           qgeocodingmanagerplugins \
           qgeoaddress \
           qgeoboundingbox \
           qgeoboundingcircle \
           qgeocoordinate \
           qgeolocation \
           qgeoplace \
           qgeocodereply \
           qgeocodingmanager \
           qplacecategory \
           qplacecontentrequest \
           qplacedescription \
           qplacemanager \
           qplaceimage \
           qplacerequest \
           qplacerating \
           qplacereview \
           qplacesearchrequest \
           qplacesupplier \
           declarative \
           sphere

contains(config_test_jsondb, yes) {
    SUBDIRS += qplacemanager_jsondb
}
