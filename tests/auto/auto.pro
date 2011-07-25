TEMPLATE = subdirs

#TODO: disable for now since
#declarative testing classes are not yet
#in the main branch of qt declarative
#SUBDIRS += declarative

CONFIG += ordered
SUBDIRS += geotestplugin \
           qgeosearchmanagerplugins \
           qgeoaddress \
           qgeoboundingbox \
           qgeoboundingcircle \
           qgeocoordinate \
           qgeolocation \
           qgeoplace \
           qgeosearchreply \
           qgeosearchmanager \
           qplacebusinessfeature \
           qplacecategory \
           qplacecontact \
           qplacedescription \
           qplacemediaobject \
           qplaceperiod \
           qplacequery \
           qplacerating \
           qplacereview \
           qplacesearchquery \
           qplacesupplier \
           qplaceweekdayhours \
           declarative
