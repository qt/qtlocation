TEMPLATE = subdirs

qtHaveModule(positioning): SUBDIRS += flickr

qtHaveModule(location) {
    SUBDIRS += places places_list places_map
    qtHaveModule(multimedia): SUBDIRS += mapviewer
}
