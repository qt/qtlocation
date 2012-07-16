TEMPLATE = subdirs

SUBDIRS = nokia

!isEmpty(QT.jsondb.name):SUBDIRS += places_jsondb
