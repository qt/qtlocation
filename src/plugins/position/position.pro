TEMPLATE = subdirs

maemo6|meego:SUBDIRS += maemo
config_geoclue:SUBDIRS += geoclue
config_gypsy:SUBDIRS += gypsy
config_locationd:!simulator:SUBDIRS += npe_backend
simulator:SUBDIRS += simulator
