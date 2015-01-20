TEMPLATE = subdirs

qtHaveModule(dbus):SUBDIRS += geoclue
config_gypsy:SUBDIRS += gypsy
qtHaveModule(simulator):SUBDIRS += simulator
ios:SUBDIRS += corelocation
android:!android-no-sdk:SUBDIRS += android
winrt:SUBDIRS += winrt

SUBDIRS += \
    positionpoll
