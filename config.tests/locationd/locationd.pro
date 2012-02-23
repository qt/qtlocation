TEMPLATE = app
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/include/mtlocationd
#unix{
#      CONFIG += link_pkgconfig
#      PKGCONFIG += locationd
#    }
LIBS += -lmtlocationd
SOURCES += main.cpp
