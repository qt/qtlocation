TEMPLATE = subdirs

qtHaveModule(quick) {
    SUBDIRS += declarative
    qtHaveModule(positioning): SUBDIRS += weatherinfo
}
qtHaveModule(positioning): qtHaveModule(widgets): SUBDIRS += logfilepositionsource
