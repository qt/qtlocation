TEMPLATE = subdirs

maemo6|meego:SUBDIRS += maemo
meego {
    contains (gypsy_enabled, yes):SUBDIRS += gypsy
}
config_geoclue:SUBDIRS += geoclue
config_locationd:!simulator:SUBDIRS += npe_backend
simulator:SUBDIRS += simulator
