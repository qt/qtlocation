TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += 3rdparty location plugins

qtHaveModule(quick): SUBDIRS += imports

