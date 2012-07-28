TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += 3rdparty location plugins

!isEmpty(QT.quick.name): SUBDIRS += imports

