TEMPLATE = app
QT += declarative
unix{
      CONFIG += link_pkgconfig
      PKGCONFIG += mt-client
    }

SOURCES += main.cpp
