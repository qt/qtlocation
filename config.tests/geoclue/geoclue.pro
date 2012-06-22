TEMPLATE = app
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += geoclue gconf-2.0
}
SOURCES += main.cpp
