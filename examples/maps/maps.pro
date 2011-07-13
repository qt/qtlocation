TEMPLATE = app
TARGET = maps

CONFIG += qt warn_on

QT += network location


RESOURCES += maps.qrc

symbian: {
    TARGET.CAPABILITY = Location \
                        NetworkServices \
                        ReadUserData \
                        WriteUserData
}

HEADERS += \
    mapswidget.h \
    marker.h \
    mainwindow.h \
    searchdialog.h \
    markerdialog.h \
    navigatedialog.h \
    navigator.h

SOURCES += \
    mapswidget.cpp \
    main.cpp \
    marker.cpp \
    mainwindow.cpp \
    searchdialog.cpp \
    markerdialog.cpp \
    navigatedialog.cpp \
    navigator.cpp

#install
target.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/maps
sources.files = $$SOURCES $HEADERS $$RESOURCES $$FORMS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/maps
INSTALLS += target sources
