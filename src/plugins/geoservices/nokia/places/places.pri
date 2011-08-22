QT *= location network script

HEADERS += \
#data classes
#parsers
    places/qplacejsonparser_p.h \
    places/qplacejsoncategoriesparser.h \
    places/qplacejsondetailsparser.h \
    places/qplacejsonmediaparser.h \
    places/qplacejsonrecommendationparser.h \
    places/qplacejsonreviewparser.h \
    places/qplacejsonsearchparser.h \
    places/qplacejsontextpredictionparser.h \
#query classes
#reply classes
    places/qplacecategoriesreplyimpl.h \
    places/qplaceimagereplyimpl.h \
    places/qplacedetailsreplyimpl.h \
    places/qplaceratingreplyimpl.h \
    places/qplacerecommendationreplyimpl.h \
    places/qplacereviewreplyimpl.h \
    places/qplacesearchreplyimpl.h \
    places/qplacetextpredictionreplyimpl.h \
#manager and engine
    qplacemanagerengine_nokia.h \
    places/qplacecategoriesrepository.h \
    places/qplacerestreply.h \
    places/qplacerestmanager.h \
    places/qplacesuppliersrepository.h \
    places/qplacecategorytree.h

SOURCES += \
#data classes
#parsers
    places/qplacejsonparser.cpp \
    places/qplacejsoncategoriesparser.cpp \
    places/qplacejsondetailsparser.cpp \
    places/qplacejsonmediaparser.cpp \
    places/qplacejsonrecommendationparser.cpp \
    places/qplacejsonreviewparser.cpp \
    places/qplacejsonsearchparser.cpp \
    places/qplacejsontextpredictionparser.cpp \
#query classes
#reply classes
    places/qplacecategoriesreplyimpl.cpp \
    places/qplacedetailsreplyimpl.cpp \
    places/qplaceimagereplyimpl.cpp \
    places/qplaceratingreplyimpl.cpp \
    places/qplacerecommendationreplyimpl.cpp \
    places/qplacereviewreplyimpl.cpp \
    places/qplacesearchreplyimpl.cpp \
    places/qplacetextpredictionreplyimpl.cpp \
#manager and engine
    qplacemanagerengine_nokia.cpp \
    places/qplacecategoriesrepository.cpp \
    places/qplacerestreply.cpp \
    places/qplacerestmanager.cpp \
    places/qplacesuppliersrepository.cpp \
    places/qplacecategorytree.cpp




