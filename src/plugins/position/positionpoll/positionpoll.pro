TARGET = qtposition_positionpoll
QT += positioning

PLUGIN_TYPE = position
load(qt_plugin)

SOURCES += \
    qgeoareamonitor_polling.cpp \
    positionpollfactory.cpp

HEADERS += \
    qgeoareamonitor_polling.h \
    positionpollfactory.h

INCLUDEPATH += $$QT.location.includes

OTHER_FILES += \
    plugin.json
