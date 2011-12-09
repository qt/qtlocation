TARGET  = declarative_location
TARGETPATH = QtLocation

include(qlocationimport.pri)

QT += declarative-private network location qtquick1 declarative qt3d core-private gui-private

DESTDIR = $$QT.location.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INCLUDEPATH += ../../location/mapsgl
INCLUDEPATH *= $$PWD

# On some platforms, build both versions because debug and release
# versions are incompatible
#win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release

HEADERS += qdeclarativeposition_p.h \
           qdeclarativepositionsource_p.h \
           qdeclarativecoordinate_p.h \
           qdeclarativegeolocation_p.h \
           qdeclarativegeomapitemview_p.h \
           qdeclarativegeomapmouseevent_p.h \
           qdeclarativegeomapmousearea_p.h \
           qdeclarativegeoserviceprovider_p.h \
           qdeclarativegeoaddress_p.h \
           qdeclarativegeoboundingarea_p.h \
           qdeclarativegeoboundingbox_p.h \
           qdeclarativegeocodemodel_p.h \
           qdeclarativegeoroutemodel_p.h \
           qdeclarativegeoroute_p.h \
           qdeclarativegeoroutesegment_p.h \
           qdeclarativegeomaneuver_p.h \
           qdeclarativegeoboundingcircle_p.h \
           qdeclarativegeomap_p.h \
           qdeclarativegeomapflickable_p.h \
           qdeclarativegeomappincharea_p.h \
           qdeclarativegeomaptype_p.h \
           qdeclarativegeomapitembase_p.h \
           qdeclarativegeomapquickitem_p.h \
           mapnode_p.h \
           qdeclarativecirclemapitem_p.h \
           qdeclarativerectanglemapitem_p.h \
           qdeclarativepolygonmapitem_p.h \
           qdeclarativepolylinemapitem_p.h \
           qdeclarativeroutemapitem_p.h

SOURCES += qdeclarativeposition.cpp \
           location.cpp \
           qdeclarativepositionsource.cpp \
           qdeclarativecoordinate.cpp \
           qdeclarativegeolocation.cpp \
           qdeclarativegeomapitemview.cpp \
           qdeclarativegeomapmouseevent.cpp \
           qdeclarativegeomapmousearea.cpp \
           qdeclarativegeoserviceprovider.cpp \
           qdeclarativegeoaddress.cpp \
           qdeclarativegeoboundingarea.cpp \
           qdeclarativegeoboundingbox.cpp \
           qdeclarativegeocodemodel.cpp \
           qdeclarativegeoroutemodel.cpp \
           qdeclarativegeoroute.cpp \
           qdeclarativegeoroutesegment.cpp \
           qdeclarativegeomaneuver.cpp \
           qdeclarativegeoboundingcircle.cpp \
           qdeclarativegeomap.cpp \
           qdeclarativegeomapflickable.cpp \
           qdeclarativegeomappincharea.cpp \
           qdeclarativegeomaptype.cpp \
           qdeclarativegeomapitembase.cpp \
           qdeclarativegeomapquickitem.cpp \
           mapnode.cpp \
           qdeclarativecirclemapitem.cpp \
           qdeclarativerectanglemapitem.cpp \
           qdeclarativepolygonmapitem.cpp \
           qdeclarativepolylinemapitem.cpp \
           qdeclarativeroutemapitem.cpp

include(declarativeplaces/declarativeplaces.pri)

# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
