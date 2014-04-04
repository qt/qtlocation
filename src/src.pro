TEMPLATE = subdirs

SUBDIRS += positioning

plugins.depends = positioning
SUBDIRS += plugins

contains(QT_CONFIG, private_tests) {
    positioning_doc_snippets.subdir = positioning/doc/snippets
    #plugin dependency required during static builds
    positioning_doc_snippets.depends = positioning plugins
    !contains(QT_BUILD_PARTS, examples) {
        positioning_doc_snippets.CONFIG = no_default_target no_default_install
    }
    SUBDIRS += positioning_doc_snippets
}

#no point in building QtLocation without Qt3D
qtHaveModule(3d) {
    SUBDIRS += 3rdparty

    location.depends = positioning 3rdparty
    SUBDIRS += location

    plugins.depends += location
    qtHaveModule(quick):imports.depends += location

    contains(QT_CONFIG, private_tests) {
        location_doc_snippets.subdir = location/doc/snippets
        #plugin dependency required during static builds
        location_doc_snippets.depends = location plugins
        !contains(QT_BUILD_PARTS, examples) {
            location_doc_snippets.CONFIG = no_default_target no_default_install
        }
        SUBDIRS += location_doc_snippets
    }
}

qtHaveModule(quick) {
    imports.depends += positioning
    SUBDIRS += imports
}
