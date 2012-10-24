TARGET = qtposition_testplugin
QT += location

PLUGIN_TYPE = position
load(qt_plugin)

SOURCES += plugin.cpp

OTHER_FILES += \
    plugin.json
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
