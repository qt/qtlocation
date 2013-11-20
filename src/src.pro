TEMPLATE = subdirs

SUBDIRS += positioning

plugins.depends = positioning
SUBDIRS += plugins

positioning_doc_snippets.subdir = positioning/doc/snippets
positioning_doc_snippets.depends = positioning
SUBDIRS += positioning_doc_snippets

#no point in building QtLocation without Qt3D
qtHaveModule(3d)|no_qt3d {
    qtHaveModule(3d):!no_qt3d {
        message(Using Qt3D Renderer)
    } else {
        DEFINES += NO_QT3D_RENDERER
        warning(Not using Qt3D as renderer backend)
    }

    location.depends = positioning 3rdparty
    SUBDIRS += location 3rdparty

    plugins.depends += location
    qtHaveModule(quick):imports.depends += location

    location_doc_snippets.subdir = location/doc/snippets
    location_doc_snippets.depends = location
    SUBDIRS += location_doc_snippets
}

qtHaveModule(quick) {
    imports.depends += positioning
    SUBDIRS += imports
}
