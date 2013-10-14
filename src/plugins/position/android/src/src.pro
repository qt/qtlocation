TARGET = qtposition_android
QT = core positioning

PLUGIN_TYPE = position
load(qt_plugin)


HEADERS = \
    positionfactory_android.h \
    qgeopositioninfosource_android_p.h \
    jnipositioning.h

SOURCES = \
    positionfactory_android.cpp \
    qgeopositioninfosource_android.cpp \
    jnipositioning.cpp

OTHER_FILES = plugin.json
