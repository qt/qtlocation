TEMPLATE = subdirs

maemo6|meego:SUBDIRS += maemo
meego {
    contains (geoclue-master_enabled, yes):SUBDIRS += geoclue
    contains (gypsy_enabled, yes):SUBDIRS += gypsy
}
contains(config_test_locationd, yes):!simulator:SUBDIRS += npe_backend
simulator:SUBDIRS += simulator
