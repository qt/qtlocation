TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += positioning

#no point in building QtLocation without Qt3D
qtHaveModule(3d) {
    SUBDIRS += location 3rdparty
}

SUBDIRS += plugins
qtHaveModule(quick): SUBDIRS += imports

