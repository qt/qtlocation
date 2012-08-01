TEMPLATE = subdirs
SUBDIRS += flickr places places_list places_map
!isEmpty(QT.multimedia.name):SUBDIRS += mapviewer
