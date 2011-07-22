#TODO: remove when plugin is refactored out
INCLUDEPATH += places

PUBLIC_HEADERS += \
#data classes
    places/qplacebusinessfeature.h \
    places/qplacebusinessinformation.h \
    places/qplacecategory.h \
    places/qplacecontact.h \
    places/qplacedescription.h \
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
    places/qplacequery.h \
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
    places/qplacemanagerenginefactory.h

PRIVATE_HEADERS += \
    places/qplacebusinessfeature_p.h \
    places/qplacebusinessinformation_p.h \
    places/qplacecategory_p.h \
    places/qplacecontact_p.h \
    places/qplacedescription_p.h \
    places/qplacemediaobject_p.h \
    places/qplaceperiod_p.h \
    places/qplacerating_p.h \
    places/qplacereview_p.h \
    places/qplacesupplier_p.h \
    places/qplaceweekdayhours_p.h \
    places/qplacesearchresult_p.h \
    places/qplacereply_p.h \
    places/qplacemanager_p.h \
    places/qplacemanagerengine_p.h

SOURCES += \
#data classes
    places/qplacebusinessfeature.cpp \
    places/qplacebusinessinformation.cpp \
    places/qplacecategory.cpp \
    places/qplacecontact.cpp \
    places/qplacedescription.cpp \
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
    places/qplacemanagerengine.cpp \
    places/qplacemanager_p.cpp
