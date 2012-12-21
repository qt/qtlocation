TEMPLATE = subdirs

SUBDIRS = nokia

qtHaveModule(jsondb): SUBDIRS += places_jsondb
