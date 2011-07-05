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
           qgeocoordinate \
           qgeolocation \
           qgeoplace \
           qgeosearchreply \
           qgeosearchmanager \
           qplacealternativevalue \
           qplacebusinessfeature \
           qplacecategory \
           qplacecontact \
           qplacedescription \
           qplacelocation \
           qplacemediaobject \
           qplaceperiod \
           qplacequery \
           qplacerating \
           qplacereview \
           qplacesearchquery \
           qplacesupplier \
           qplaceweekdayhours
