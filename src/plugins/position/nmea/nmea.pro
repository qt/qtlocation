TARGET = qtposition_nmea

QT = core-private positioning-private serialport

HEADERS += \
    qgeopositioninfosourcefactory_nmea.h qiopipe_p.h

SOURCES += \
    qgeopositioninfosourcefactory_nmea.cpp qiopipe.cpp

OTHER_FILES += \
    plugin.json

PLUGIN_TYPE = position
PLUGIN_CLASS_NAME = QGeoPositionInfoSourceFactoryNmea
load(qt_plugin)
