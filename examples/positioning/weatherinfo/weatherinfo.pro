TEMPLATE = app
TARGET = weatherinfo

QT += core network positioning qml quick

CONFIG += qmltypes
QML_IMPORT_NAME = WeatherInfo
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += main.cpp \
    appmodel.cpp

OTHER_FILES += weatherinfo.qml \
    components/WeatherIcon.qml \
    components/ForecastIcon.qml \
    components/BigForecastIcon.qml \
    icons/*


RESOURCES += weatherinfo.qrc

HEADERS += appmodel.h

target.path = $$[QT_INSTALL_EXAMPLES]/positioning/weatherinfo
INSTALLS += target
