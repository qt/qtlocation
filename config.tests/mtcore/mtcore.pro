TEMPLATE = app

QT += declarative
unix{
      CONFIG += link_pkgconfig
      PKGCONFIG += mtcore
    }

SOURCES += main.cpp
