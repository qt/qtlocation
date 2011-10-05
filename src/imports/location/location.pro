TARGET  = declarative_location
TARGETPATH = Qt/location

# Compatibility demands old path too.
#TARGETPATH = QtMobility/location

include(qlocationimport.pri)

QT += declarative-private network location qtquick1 declarative
CONFIG += qt3d

DESTDIR = $$QT.location.imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

#INCLUDEPATH += ../../../../qtdeclarative/include
INCLUDEPATH += ../../../../qtdeclarative/include/QtDeclarative/5.0.0/QtDeclarative
INCLUDEPATH += ../../../../qtdeclarative/include/QtDeclarative/5.0.0/QtDeclarative/private
INCLUDEPATH *= $$PWD

# On some platforms, build both versions because debug and release
# versions are incompatible
#win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release

HEADERS += qdeclarativeposition_p.h \
           qdeclarativepositionsource_p.h \
           qdeclarativecoordinate_p.h \
           qdeclarativegeolocation_p.h \
           qdeclarativegeomapobjectview_p.h \
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
           qdeclarative3dgraphicsgeomap_p.h \
           qdeclarativegeomapflickable_p.h \
           qdeclarativegeomappincharea_p.h \
           qdeclarativegeomapitem_p.h

SOURCES += qdeclarativeposition.cpp \
           location.cpp \
           qdeclarativepositionsource.cpp \
           qdeclarativecoordinate.cpp \
           qdeclarativegeolocation.cpp \
           qdeclarativegeomapobjectview.cpp \
           qdeclarativegeomapmouseevent.cpp \
           qdeclarativegeomapmousearea.cpp \
           qdeclarativegeoserviceprovider.cpp \
           qdeclarativegeoaddress.cpp \
           qdeclarativegeoboundingbox.cpp \
           qdeclarativegeocodemodel.cpp \
           qdeclarativegeoroutemodel.cpp \
           qdeclarativegeoroute.cpp \
           qdeclarativegeoroutesegment.cpp \
           qdeclarativegeomaneuver.cpp \
           qdeclarativegeoboundingcircle.cpp \
           qdeclarative3dgraphicsgeomap.cpp \
           qdeclarativegeomapflickable.cpp \
           qdeclarativegeomappincharea.cpp \
           qdeclarativegeomapitem.cpp

include(declarativeplaces/declarativeplaces.pri)

# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

# This is interim hack until the QSGMouseArea and QSGPinchArea are enabled.
# Hack makes code compile, but for anything useful you want the
# QSGMOUSEAREA_AVAILABLE
if (exists($$PWD/qsgmousearea_p.h)) {
   message('Will try to build QML Map 3D with QML2 user interaction elements and Map Items.')
   DEFINES += QSGMOUSEAREA_AVAILABLE
   DEFINES += QSGSHADEREFFECTSOURCE_AVAILABLE
} else {
   message('QML2 3D Map user interaction elements and MapItems not available. Check location.pro - file for instructions.')
#  1) Modify qsgmousearea_p.h at qtdeclaratice/src/declarative/items
#  Make the QSGMouseArea exported instead of autotest exported, i.e. the file should have:
#  class Q_DECLARATIVE_EXPORT QSGMouseArea : public QSGItem
#  2) Also export the QSGShaderEffectSource in qsgshadereffectsource_p.h
#  3) Make the declarative module.
#  4) Rerun qmake and make
}
