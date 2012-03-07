load(qt_plugin)

TARGET = qtposition_testplugin
QT += location

DESTDIR = $$QT.location.plugins/position

SOURCES += plugin.cpp

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += \
    plugin.json
