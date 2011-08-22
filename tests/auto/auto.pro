TEMPLATE = subdirs

#TODO: disable for now since
#declarative testing classes are not yet
#in the main branch of qt declarative
#SUBDIRS += declarative

CONFIG += ordered
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
           declarative
