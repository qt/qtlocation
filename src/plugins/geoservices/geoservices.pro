TEMPLATE = subdirs

SUBDIRS = nokia

!isEmpty(QT.jsondb.name):SUBDIRS += nokia_places_jsondb
