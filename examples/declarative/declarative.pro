TEMPLATE = subdirs
SUBDIRS += flickr places
!isEmpty(QT.multimedia.name):SUBDIRS += mapviewer
