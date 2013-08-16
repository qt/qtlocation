TEMPLATE = subdirs

qtHaveModule(positioning): SUBDIRS += positioning
qtHaveModule(3d):qtHaveModule(location): SUBDIRS += location

