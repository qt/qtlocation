QT += quick-private network location-private qml-private 3d core-private gui-private v8-private

INCLUDEPATH += ../../location
INCLUDEPATH += ../../location/maps
INCLUDEPATH *= $$PWD
DEFINES += TOUCH_EVENT_WORKAROUND

LIBS += -L../../3rdparty/poly2tri -lpoly2tri

win32 {
    CONFIG(debug, debug|release) {
        LIBS += -L../../3rdparty/poly2tri/debug
    } else {
        LIBS += -L../../3rdparty/poly2tri/release
    }
}

HEADERS += qdeclarativeposition_p.h \
           qdeclarativepositionsource_p.h \
           qdeclarativecoordinate_p.h \
           qdeclarativegeolocation_p.h \
           qdeclarativegeomapitemview_p.h \
           qdeclarativegeomapmouseevent_p.h \
           qdeclarativegeomapmousearea_p.h \
           qdeclarativegeoserviceprovider_p.h \
           qdeclarativegeoaddress_p.h \
           qdeclarativegeoshape.h \
           qdeclarativegeorectangle.h \
           qdeclarativegeocircle.h \
           qdeclarativegeocodemodel_p.h \
           qdeclarativegeoroutemodel_p.h \
           qdeclarativegeoroute_p.h \
           qdeclarativegeoroutesegment_p.h \
           qdeclarativegeomaneuver_p.h \
           qdeclarativegeomap_p.h \
           qdeclarativegeomaptype_p.h \
           qdeclarativegeomapitembase_p.h \
           qdeclarativegeomapquickitem_p.h \
           mapnode_p.h \
           qdeclarativecirclemapitem_p.h \
           qdeclarativerectanglemapitem_p.h \
           qdeclarativepolygonmapitem_p.h \
           qdeclarativepolylinemapitem_p.h \
           qdeclarativeroutemapitem_p.h \
           qgeomapitemgeometry_p.h \
           qdeclarativegeomapcopyrightsnotice_p.h \
           qdeclarativegeomapgesturearea_p.h \
           error_messages.h \
           locationvaluetypeprovider.h \
           locationsingleton.h

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
           qdeclarativegeoshape.cpp \
           qdeclarativegeorectangle.cpp \
           qdeclarativegeocircle.cpp \
           qdeclarativegeocodemodel.cpp \
           qdeclarativegeoroutemodel.cpp \
           qdeclarativegeoroute.cpp \
           qdeclarativegeoroutesegment.cpp \
           qdeclarativegeomaneuver.cpp \
           qdeclarativegeomap.cpp \
           qdeclarativegeomaptype.cpp \
           qdeclarativegeomapitembase.cpp \
           qdeclarativegeomapquickitem.cpp \
           mapnode.cpp \
           qdeclarativecirclemapitem.cpp \
           qdeclarativerectanglemapitem.cpp \
           qdeclarativepolygonmapitem.cpp \
           qdeclarativepolylinemapitem.cpp \
           qdeclarativeroutemapitem.cpp \
           qgeomapitemgeometry.cpp \
           qdeclarativegeomapcopyrightsnotice.cpp \
           qdeclarativegeomapgesturearea.cpp \
           error_messages.cpp \
           locationvaluetypeprovider.cpp \
           locationsingleton.cpp

include(declarativeplaces/declarativeplaces.pri)

load(qml_plugin)

OTHER_FILES += \
    plugin.json \
    qmldir
