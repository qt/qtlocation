load(qt_module)

TARGET = qtposition_simulator
QT += location gui

load(qt_plugin)

DESTDIR = $$QT.location.plugins/position
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"

INCLUDEPATH += $$QT.location.includes

QT += simulator
DEFINES += QT_SIMULATOR
SOURCES += qgeopositioninfosource_simulator.cpp \
            qlocationdata_simulator.cpp \
            qgeosatelliteinfosource_simulator.cpp \
            qlocationconnection_simulator.cpp \
    qgeopositioninfosourcefactory_simulator.cpp
HEADERS += qgeopositioninfosource_simulator_p.h \
            qlocationdata_simulator_p.h \
            qgeosatelliteinfosource_simulator_p.h \
            qlocationconnection_simulator_p.h \
    qgeopositioninfosourcefactory_simulator.h

target.path += $$[QT_INSTALL_PLUGINS]/position
INSTALLS += target

OTHER_FILES += \
    plugin.json
