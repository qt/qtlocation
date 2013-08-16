TEMPLATE = subdirs
qtHaveModule(positioning): SUBDIRS +=  position
qtHaveModule(3d):qtHaveModule(location) SUBDIRS += geoservices
