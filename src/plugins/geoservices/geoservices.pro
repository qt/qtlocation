TEMPLATE = subdirs

SUBDIRS = nokia osm

qtHaveModule(jsondb): SUBDIRS += places_jsondb
