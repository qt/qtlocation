TEMPLATE = subdirs

qtHaveModule(dbus):SUBDIRS += geoclue
config_gypsy:SUBDIRS += gypsy
qtHaveModule(simulator):SUBDIRS += simulator
osx|ios:SUBDIRS += corelocation
android:!android-no-sdk:SUBDIRS += android
winrt:SUBDIRS += winrt
win32:qtHaveModule(serialport):SUBDIRS += serialnmea

SUBDIRS += \
    positionpoll
