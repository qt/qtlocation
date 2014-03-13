TEMPLATE = subdirs

SUBDIRS += positioning

plugins.depends = positioning
SUBDIRS += plugins

positioning_doc_snippets.subdir = positioning/doc/snippets
positioning_doc_snippets.depends = positioning
SUBDIRS += positioning_doc_snippets

qtHaveModule(quick) {
    SUBDIRS += 3rdparty

    location.depends = positioning 3rdparty
    SUBDIRS += location

    plugins.depends += location
    imports.depends += location

    location_doc_snippets.subdir = location/doc/snippets
    location_doc_snippets.depends = location
    SUBDIRS += location_doc_snippets

    imports.depends += positioning
    SUBDIRS += imports
}
