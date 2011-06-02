#TODO: remove when plugin is refactored out
QT += declarative
QT += script

INCLUDEPATH += places

PUBLIC_HEADERS += \
#data classes
    places/qplace.h \
    places/qplaceaddress.h \
    places/qplacealternativevalue.h \
    places/qplacebusinessfeature.h \
    places/qplacebusinessinformation.h \
    places/qplacecategory.h \
    places/qplacecontact.h \
    places/qplacedescription.h \
    places/qplacegeoboundingbox.h \
    places/qplacegeocoordinate.h \
    places/qplacelocation.h \
    places/qplacemediaobject.h \
    places/qplacepaginationlist.h \
    places/qplaceperiod.h \
    places/qplacerating.h \
    places/qplacereview.h \
    places/qplacesupplier.h \
    places/qplaceweekdayhours.h \
#result
    places/qplacesearchresult.h \
#query classes
    places/placequery.h \
    places/qplacesearchquery.h \
#reply classes
    places/qplacereply.h \
    places/qplacedetailsreply.h \
    places/qplacemediareply.h \
    places/qplacereviewreply.h \
    places/qplacesearchreply.h \
    places/qplacetextpredictionreply.h \
#manager and engine
    places/qplacemanager.h \
    places/qplacemanagerengine.h \
    places/qplaceglobal.h

PRIVATE_HEADERS += \
    places/qplace_p.h \
    places/qplaceaddress_p.h \
    places/qplacealternativevalue_p.h \
    places/qplacebusinessfeature_p.h \
    places/qplacebusinessinformation_p.h \
    places/qplacecategory_p.h \
    places/qplacecontact_p.h \
    places/qplacedescription_p.h \
    places/qplacegeoboundingbox_p.h \
    places/qplacegeocoordinate_p.h \
    places/qplacelocation_p.h \
    places/qplacemediaobject_p.h \
    places/qplaceperiod_p.h \
    places/qplacerating_p.h \
    places/qplacereview_p.h \
    places/qplacesupplier_p.h \
    places/qplaceweekdayhours_p.h \
    places/qplacesearchresult_p.h

SOURCES += \
#data classes
    places/qplace.cpp \
    places/qplaceaddress.cpp \
    places/qplacealternativevalue.cpp \
    places/qplacebusinessfeature.cpp \
    places/qplacebusinessinformation.cpp \
    places/qplacecategory.cpp \
    places/qplacecontact.cpp \
    places/qplacedescription.cpp \
    places/qplacegeoboundingbox.cpp \
    places/qplacegeocoordinate.cpp \
    places/qplacelocation.cpp \
    places/qplacemediaobject.cpp \
    places/qplaceperiod.cpp \
    places/qplacerating.cpp \
    places/qplacereview.cpp \
    places/qplacesupplier.cpp \
    places/qplaceweekdayhours.cpp \
#result
    places/qplacesearchresult.cpp \
#query classes
    places/qplacequery.cpp \
    places/qplacesearchquery.cpp \
#reply classes
    places/qplacereply.cpp \
    places/qplacedetailsreply.cpp \
    places/qplacemediareply.cpp \
    places/qplacereviewreply.cpp \
    places/qplacesearchreply.cpp \
    places/qplacetextpredictionreply.cpp \
#manager and engine
    places/qplacemanager.cpp \
    places/qplacemanagerengine.cpp

#provider implementation
SOURCES += \
#data classes
#parsers
    places/provider/qplacejsoncategoriesparser.cpp \
    places/provider/qplacejsondetailsparser.cpp \
    places/provider/qplacejsonmediaparser.cpp \
    places/provider/qplacejsonrecommendationparser.cpp \
    places/provider/qplacejsonreviewparser.cpp \
    places/provider/qplacejsonsearchparser.cpp \
    places/provider/qplacejsontextpredictionparser.cpp \
#query classes
#reply classes
    places/provider/qplacedetailsreplyimpl.cpp \
    places/provider/qplacemediareplyimpl.cpp \
    places/provider/qplaceratingreplyimpl.cpp \
    places/provider/qplacerecommendationreplyimpl.cpp \
    places/provider/qplacereviewreplyimpl.cpp \
    places/provider/qplacesearchreplyimpl.cpp \
    places/provider/qplacetextpredictionreplyimpl.cpp \
#manager and engine
    places/provider/qplacemanagerengineimpl.cpp \
    places/provider/qplacecategoriesrepository.cpp \
    places/provider/qplacerestreply.cpp \
    places/provider/qplacerestmanager.cpp \
    places/provider/qplacesuppliersrepository.cpp

HEADERS += \
#data classes
#parsers
    places/provider/qplacejsoncategoriesparser.h \
    places/provider/qplacejsondetailsparser.h \
    places/provider/qplacejsonmediaparser.h \
    places/provider/qplacejsonrecommendationparser.h \
    places/provider/qplacejsonreviewparser.h \
    places/provider/qplacejsonsearchparser.h \
    places/provider/qplacejsontextpredictionparser.h \
#query classes
#reply classes
    places/provider/qplacemediareplyimpl.h \
    places/provider/qplacedetailsreplyimpl.h \
    places/provider/qplaceratingreplyimpl.h \
    places/provider/qplacerecommendationreplyimpl.h \
    places/provider/qplacereviewreplyimpl.h \
    places/provider/qplacesearchreplyimpl.h \
    places/provider/qplacetextpredictionreplyimpl.h \
#manager and engine
    places/provider/qplacemanagerengineimpl.h \
    places/provider/qplacecategoriesrepository.h \
    places/provider/qplacerestreply.h \
    places/provider/qplacerestmanager.h \
    places/provider/qplacesuppliersrepository.h
