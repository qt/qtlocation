TEMPLATE = app
TARGET = weatherinfo

QT += core network positioning qml quick

SOURCES += main.cpp \
    appmodel.cpp

OTHER_FILES += weatherinfo.qml \
    components/WeatherIcon.qml \
    components/ForecastIcon.qml \
    components/BigForecastIcon.qml

RESOURCES += weatherinfo.qrc

HEADERS += appmodel.h

#install
target.path = $$[QT_INSTALL_EXAMPLES]/qtpositioning/weatherinfo
sources.files = $$SOURCES $HEADERS $$RESOURCES $$FORMS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtpositioning/weatherinfo
INSTALLS += target sources
