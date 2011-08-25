TEMPLATE = subdirs

SUBDIRS = nokia

contains(config_test_jsondb, yes) {
          SUBDIRS += nokia_places_jsondb
}
