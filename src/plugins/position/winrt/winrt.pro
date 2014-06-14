TARGET = qtposition_winrt
QT = core positioning

PLUGIN_TYPE = position
load(qt_plugin)

INCLUDEPATH += $$QT.location.includes

SOURCES += qgeopositioninfosource_winrt.cpp \
    qgeopositioninfosourcefactory_winrt.cpp
HEADERS += qgeopositioninfosource_winrt_p.h \
    qgeopositioninfosourcefactory_winrt.h

OTHER_FILES += \
    plugin.json
