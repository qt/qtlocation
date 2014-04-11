TEMPLATE = subdirs

SUBDIRS += positioning

plugins.depends = positioning
SUBDIRS += plugins

#TEMPORARY fix for static build errors (QTBUG-37638)
#positioning_doc_snippets.subdir = positioning/doc/snippets
#positioning_doc_snippets.depends = positioning
#SUBDIRS += positioning_doc_snippets

#no point in building QtLocation without Qt3D
qtHaveModule(3d) {
    SUBDIRS += 3rdparty

    location.depends = positioning 3rdparty
    SUBDIRS += location

    plugins.depends += location
    qtHaveModule(quick):imports.depends += location

#    location_doc_snippets.subdir = location/doc/snippets
#    location_doc_snippets.depends = location
#    SUBDIRS += location_doc_snippets
}

qtHaveModule(quick) {
    imports.depends += positioning
    SUBDIRS += imports
}
