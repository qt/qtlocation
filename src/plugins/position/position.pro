TEMPLATE = subdirs

config_geoclue:SUBDIRS += geoclue
config_gypsy:SUBDIRS += gypsy
simulator:SUBDIRS += simulator
blackberry:SUBDIRS += blackberry
ios:SUBDIRS += corelocation
android:!android-no-sdk:SUBDIRS += android

SUBDIRS += \
    positionpoll
