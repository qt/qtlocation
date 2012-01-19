QT *= location network declarative

HEADERS += \
#data classes
#parsers
    placesv1/qplacejsonparser_p.h \
    placesv1/qplacejsoncategoriesparser.h \
    placesv1/qplacejsondetailsparser.h \
    placesv1/qplacejsonmediaparser.h \
    placesv1/qplacejsonrecommendationparser.h \
    placesv1/qplacejsonreviewparser.h \
    placesv1/qplacejsonsearchparser.h \
    placesv1/qplacejsontextpredictionparser.h \
#query classes
#reply classes
    placesv1/qplaceratingreplyimpl.h \
    placesv1/qplacetextpredictionreplyimpl.h \
#manager and engine
    qplacemanagerengine_nokiav1.h \
    placesv1/qplacecategoriesrepository.h \
    placesv1/qplacerestreply.h \
    placesv1/qplacerestmanager.h \
    placesv1/qplacesuppliersrepository.h \
#misc
    placesv1/unsupportedreplies.h \
    placesv1/qplacecategoriesreplyimplv1.h \
    placesv1/qplacecontentreplyimplv1.h \
    placesv1/qplacedetailsreplyimplv1.h \
    placesv1/qplacerecommendationreplyimplv1.h \
    placesv1/qplacesearchreplyimplv1.h

SOURCES += \
#data classes
#parsers
    placesv1/qplacejsonparser.cpp \
    placesv1/qplacejsoncategoriesparser.cpp \
    placesv1/qplacejsondetailsparser.cpp \
    placesv1/qplacejsonmediaparser.cpp \
    placesv1/qplacejsonrecommendationparser.cpp \
    placesv1/qplacejsonreviewparser.cpp \
    placesv1/qplacejsonsearchparser.cpp \
    placesv1/qplacejsontextpredictionparser.cpp \
#query classes
#reply classes
    placesv1/qplaceratingreplyimpl.cpp \
    placesv1/qplacetextpredictionreplyimpl.cpp \
#manager and engine
    qplacemanagerengine_nokiav1.cpp \
    placesv1/qplacecategoriesrepository.cpp \
    placesv1/qplacerestreply.cpp \
    placesv1/qplacerestmanager.cpp \
    placesv1/qplacesuppliersrepository.cpp \
    placesv1/qplacecategoriesreplyimplv1.cpp \
    placesv1/qplacecontentreplyimplv1.cpp \
    placesv1/qplacedetailsreplyimplv1.cpp \
    placesv1/qplacerecommendationreplyimplv1.cpp \
    placesv1/qplacesearchreplyimplv1.cpp




