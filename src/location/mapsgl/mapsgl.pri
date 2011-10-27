
INCLUDEPATH += mapsgl

QT += network qt3d

include(map2d/map2d.pri)
include(map3d/map3d.pri)

SOURCES += \
    mapsgl/cameradata.cpp \
    mapsgl/frustum.cpp \
    mapsgl/map.cpp \
    mapsgl/mapitem.cpp \
    mapsgl/mapitemtree.cpp \
    mapsgl/mapsphere.cpp \
    mapsgl/projection.cpp \
    mapsgl/tilecache.cpp \
    mapsgl/tile.cpp \
    mapsgl/tilespec.cpp

PUBLIC_HEADERS += \
    mapsgl/cameradata.h \
    mapsgl/map.h \
    mapsgl/mapitem.h \
    mapsgl/tilecache.h \
    mapsgl/tile.h \
    mapsgl/tilespec.h

PRIVATE_HEADERS += \
    mapsgl/frustum_p.h \
    mapsgl/intervaltree_p.h \
    mapsgl/map_p.h \
    mapsgl/mapitemtree_p.h \
    mapsgl/mapsphere_p.h \
    mapsgl/projection_p.h
