TARGET = qtposition_serialnmea

QT = core-private positioning-private serialport

HEADERS += \
    qgeopositioninfosourcefactory_serialnmea.h qiopipe_p.h

SOURCES += \
    qgeopositioninfosourcefactory_serialnmea.cpp qiopipe.cpp

OTHER_FILES += \
    plugin.json

PLUGIN_TYPE = position
PLUGIN_CLASS_NAME = QGeoPositionInfoSourceFactorySerialNmea
load(qt_plugin)
