TEMPLATE = subdirs

!isEmpty(QT.quick.name): SUBDIRS += declarative weatherinfo
!contains(QT_CONFIG, no-widgets): SUBDIRS += logfilepositionsource
