TEMPLATE = subdirs
SUBDIRS += flickr places places_list places_map
qtHaveModule(multimedia): SUBDIRS += mapviewer
