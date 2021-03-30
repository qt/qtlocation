TARGET = qtposition_testplugin2
QT += positioning-private

PLUGIN_TYPE = position
PLUGIN_CLASS_NAME = DummyPluginForTests
PLUGIN_EXTENDS = -
load(qt_plugin)

SOURCES += plugin.cpp

OTHER_FILES += \
    plugin.json
