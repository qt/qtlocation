TARGET  = declarative_location
TARGETPATH = QtLocation

include(qlocationimport.pri)

QT += quick-private network location qml-private 3d core-private gui-private

DESTDIR = $$QT.location.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INCLUDEPATH += ../../location/maps
INCLUDEPATH *= $$PWD
DEFINES += TOUCH_EVENT_WORKAROUND

LIBS += -L../../3rdparty/poly2tri -lpoly2tri
win32:LIBS += -L../../3rdparty/poly2tri/release -L../../3rdparty/poly2tri/debug

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
           qdeclarativeroutemapitem_p.h \
           qgeomapitemgeometry_p.h \
           qdeclarativegeomapcopyrightsnotice_p.h \
    qdeclarativegeomapgesturearea_p.h

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
           qdeclarativeroutemapitem.cpp \
           qgeomapitemgeometry.cpp \
           qdeclarativegeomapcopyrightsnotice.cpp \
    qdeclarativegeomapgesturearea.cpp

include(declarativeplaces/declarativeplaces.pri)

# plugin.qmltypes is used by Qt Creator for syntax highlighting and the QML code model.  It needs
# to be regenerated whenever the QML elements exported by the plugin change.  This cannot be done
# automatically at compile time because qmlplugindump does not support some QML features and it may
# not be possible when cross-compiling.
#
# To regenerate run 'make qmltypes' which will update the plugins.qmltypes file in the source
# directory.  Then review and commit the changes made to plugins.qmltypes.
#
# This will run the following command:
#     qmlplugindump <import name> <import version> <path to import plugin> > plugins.qmltypes
# e.g.:
#     qmlplugindump QtLocation 5.0 imports/QtLocation/libdeclarative_location.so > plugins.qmltypes

load(resolve_target)
qmltypes.target = qmltypes
qmltypes.commands = $$[QT_INSTALL_BINS]/qmlplugindump QtLocation 5.0 $$QMAKE_RESOLVED_TARGET > $$PWD/plugins.qmltypes
qmltypes.depends = $$QMAKE_RESOLVED_TARGET
QMAKE_EXTRA_TARGETS += qmltypes

# Tell qmake to create such makefile that qmldir, plugins.qmltypes and target
# (i.e. declarative_location) are all copied to $$[QT_INSTALL_IMPORTS]/QtLocation directory,

qmldir.files += $$PWD/qmldir $$PWD/plugins.qmltypes
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

OTHER_FILES += \
    plugin.json

