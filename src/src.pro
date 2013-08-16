TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += 3rdparty positioning location plugins

qtHaveModule(quick): SUBDIRS += imports

