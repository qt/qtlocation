load(qt_module)

TARGET = qtplaces_nokia
QT += location network script

include($$QT_SOURCE_TREE/src/plugins/qpluginbase.pri)

DESTDIR = $$QT.location.plugins/places
#QTDIR_build:REQUIRES += "contains(QT_CONFIG, location)"

HEADERS += \
#data classes
#parsers
    qplacejsonparser_p.h \
    qplacejsoncategoriesparser.h \
    qplacejsondetailsparser.h \
    qplacejsonmediaparser.h \
    qplacejsonrecommendationparser.h \
    qplacejsonreviewparser.h \
    qplacejsonsearchparser.h \
    qplacejsontextpredictionparser.h \
#query classes
#reply classes
    qplacecategoriesreplyimpl.h \
    qplacemediareplyimpl.h \
    qplacedetailsreplyimpl.h \
    qplaceratingreplyimpl.h \
    qplacerecommendationreplyimpl.h \
    qplacereviewreplyimpl.h \
    qplacesearchreplyimpl.h \
    qplacetextpredictionreplyimpl.h \
#manager and engine
    qplacemanagerengineimpl.h \
    qplacecategoriesrepository.h \
    qplacerestreply.h \
    qplacerestmanager.h \
    qplacesuppliersrepository.h \
    placemanagerenginefactory_nokia.h

SOURCES += \
#data classes
#parsers
    qplacejsonparser.cpp \
    qplacejsoncategoriesparser.cpp \
    qplacejsondetailsparser.cpp \
    qplacejsonmediaparser.cpp \
    qplacejsonrecommendationparser.cpp \
    qplacejsonreviewparser.cpp \
    qplacejsonsearchparser.cpp \
    qplacejsontextpredictionparser.cpp \
#query classes
#reply classes
    qplacecategoriesreplyimpl.cpp \
    qplacedetailsreplyimpl.cpp \
    qplacemediareplyimpl.cpp \
    qplaceratingreplyimpl.cpp \
    qplacerecommendationreplyimpl.cpp \
    qplacereviewreplyimpl.cpp \
    qplacesearchreplyimpl.cpp \
    qplacetextpredictionreplyimpl.cpp \
#manager and engine
    qplacemanagerengineimpl.cpp \
    qplacecategoriesrepository.cpp \
    qplacerestreply.cpp \
    qplacerestmanager.cpp \
    qplacesuppliersrepository.cpp \
    placemanagerenginefactory_nokia.cpp


INCLUDEPATH += $$QT.location.includes

message($$QT.location.includes)

#INCLUDEPATH += $$SOURCE_DIR/src/location \
#                $$SOURCE_DIR/src/location/maps \
#                $$SOURCE_DIR/src/location/maps/tiled

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    #TODO: Fill in uid3 id for Symbian
    TARGET.UID3 =
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep
    LIBS += -lefsrv
}

target.path += $$[QT_INSTALL_PLUGINS]/places
INSTALLS += target

