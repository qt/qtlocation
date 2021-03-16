TEMPLATE = app
TARGET = satelliteinfo

QT += quick positioning

CONFIG += qmltypes
QML_IMPORT_NAME = Local
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += main.cpp \
    satellitemodel.cpp

HEADERS += \
    satellitemodel.h

OTHER_FILES += \
    satelliteinfo.qml

RESOURCES += \
    satelliteinfo.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/positioning/satelliteinfo
INSTALLS += target



