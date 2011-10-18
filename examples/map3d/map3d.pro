
TEMPLATE = app
TARGET = map3d

CONFIG += qt warn_on

QT += network location qt3d widgets

INCLUDEPATH += ../../src/location/mapsgl/map3d

HEADERS += \
    camerawidget.h \
    mainwidget.h \
    tileview.h

SOURCES += \
    main.cpp \
    camerawidget.cpp \
    mainwidget.cpp \
    tileview.cpp

#install
target.path = $$[QT_INSTALL_DEMOS]/qtlocation/map3d
sources.files = $$SOURCES $HEADERS $$RESOURCES $$FORMS *.pro
sources.path = $$[QT_INSTALL_DEMOS]/qtlocation/map3d
INSTALLS += target sources
