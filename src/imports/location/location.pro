TARGET  = declarative_location
TARGETPATH = Qt/location

# Compatibility demands old path too.
#TARGETPATH = QtMobility/location

include(qlocationimport.pri)

QT += declarative network location

DESTDIR = $$QT.location.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH


# On some platforms, build both versions because debug and release
# versions are incompatible
#win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release

# Where to search for the #include'd files
#INCLUDEPATH += ../../../src/global
#INCLUDEPATH += ../../../src/location
#INCLUDEPATH += ../../../src/location/landmarks
#INCLUDEPATH += ../../../src/location/maps
#INCLUDEPATH += .

HEADERS += qdeclarativeposition_p.h \
           qdeclarativepositionsource_p.h \
           qdeclarativelandmark_p.h \
           qdeclarativelandmarkmodel_p.h \
           qdeclarativelandmarkcategorymodel_p.h \
           qdeclarativelandmarkcategory_p.h \
           qdeclarativelandmarkfilters_p.h \
           qdeclarativecoordinate_p.h \
           qdeclarativegeomapobjectborder_p.h \
           qdeclarativegeomapcircleobject_p.h \
           qdeclarativegeomapgroupobject_p.h \
           qdeclarativegeomapobject_p.h \
           qdeclarativegeomappixmapobject_p.h \
           qdeclarativegeomappolygonobject_p.h \
           qdeclarativegeomappolylineobject_p.h \
           qdeclarativegeomaprectangleobject_p.h \
           qdeclarativegeomaptextobject_p.h \
           qdeclarativegeomapmouseevent_p.h \
           qdeclarativegeomapmousearea_p.h \
           qdeclarativegeoserviceprovider_p.h \
           qdeclarativegraphicsgeomap_p.h \
           qdeclarativegeoplace_p.h \
           qdeclarativegeoaddress_p.h \
           qdeclarativegeoboundingbox_p.h \
           qdeclarativegeocodemodel_p.h \
           qdeclarativegeoroutemodel_p.h \
           qdeclarativegeoroute_p.h \
           qdeclarativegeoroutesegment_p.h \
           qdeclarativegeomaneuver_p.h \
           qdeclarativegeomaprouteobject_p.h \
           qdeclarativegeoboundingcircle_p.h

SOURCES += qdeclarativeposition.cpp \
           location.cpp \
           qdeclarativepositionsource.cpp \
           qdeclarativelandmark.cpp \
           qdeclarativelandmarkmodel.cpp \
           qdeclarativelandmarkcategorymodel.cpp \
           qdeclarativelandmarkcategory.cpp \
           qdeclarativelandmarkfilters.cpp \
           qdeclarativecoordinate.cpp \
           qdeclarativegeomapobjectborder.cpp \
           qdeclarativegeomapcircleobject.cpp \
           qdeclarativegeomapgroupobject.cpp \
           qdeclarativegeomapobject.cpp \
           qdeclarativegeomappixmapobject.cpp \
           qdeclarativegeomappolygonobject.cpp \
           qdeclarativegeomappolylineobject.cpp \
           qdeclarativegeomaprectangleobject.cpp \
           qdeclarativegeomaptextobject.cpp \
           qdeclarativegeomapmouseevent.cpp \
           qdeclarativegeomapmousearea.cpp \
           qdeclarativegeoserviceprovider.cpp \
           qdeclarativegraphicsgeomap.cpp \
           qdeclarativegeoplace.cpp \
           qdeclarativegeoaddress.cpp \
           qdeclarativegeoboundingbox.cpp \
           qdeclarativegeocodemodel.cpp \
           qdeclarativegeoroutemodel.cpp \
           qdeclarativegeoroute.cpp \
           qdeclarativegeoroutesegment.cpp \
           qdeclarativegeomaneuver.cpp \
           qdeclarativegeomaprouteobject.cpp \
           qdeclarativegeoboundingcircle.cpp


# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
