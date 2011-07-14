TEMPLATE=app
TARGET = qml_location_landmarkmap

QT += qtquick1 declarative network location

SOURCES += qmllandmarkmap.cpp

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.CAPABILITY += LocalServices ReadDeviceData WriteDeviceData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    landmarkmap.qrc

install_files.files += nmealog.txt mylm.lmx landmarkmapcommon landmarkmapmobile landmarkmap.qml
install_files.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/landmark

target.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/landmark
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/declarative/landmark

INSTALLS += target sources install_files
