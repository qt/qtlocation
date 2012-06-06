TEMPLATE = subdirs
SUBDIRS += declarative weatherinfo
!contains(QT_CONFIG, no-widgets): SUBDIRS += logfilepositionsource
