TEMPLATE = subdirs

qtHaveModule(dbus):SUBDIRS += geoclue
config_gypsy:SUBDIRS += gypsy
config_winrt:SUBDIRS += winrt
qtHaveModule(simulator):SUBDIRS += simulator
osx|ios|tvos:SUBDIRS += corelocation
android:SUBDIRS += android
win32:qtHaveModule(serialport):SUBDIRS += serialnmea

SUBDIRS += \
    positionpoll
