#TODO: remove when plugin is refactored out
INCLUDEPATH += places

PUBLIC_HEADERS += \
    places/placemacro.h \
#data classes
    places/qplaceattribute.h \
    places/qplacecategory.h \
    places/qplacecontent.h \
    places/qplacecontentreply.h \
    places/qplaceeditorial.h \
    places/qplaceimage.h \
    places/qplaceicon.h \
    places/qplacerating.h \
    places/qplacereview.h \
    places/qplacesupplier.h \
    places/qplaceuser.h \
#result
    places/qplacesearchresult.h \
#request classes
    places/qplacecontentrequest.h \
    places/qplacerequest.h \
    places/qplacesearchrequest.h \
#reply classes
    places/qplacereply.h \
    places/qplacedetailsreply.h \
    places/qplaceidreply.h \
    places/qplacesearchreply.h \
    places/qplacetextpredictionreply.h \
#manager and engine
    places/qplacemanager.h \
    places/qplacemanagerengine.h

PRIVATE_HEADERS += \
    places/qplaceattribute_p.h \
    places/qplacecategory_p.h \
    places/qplacecontent_p.h \
    places/qplaceeditorial_p.h \
    places/qplaceicon_p.h \
    places/qplaceimage_p.h \
    places/qplacerating_p.h \
    places/qplacereview_p.h \
    places/qplacesupplier_p.h \
    places/qplacesearchresult_p.h \
    places/qplacereply_p.h \
    places/qplacemanagerengine_p.h \
    places/qplacerequest_p.h \
    places/qplacecontentrequest_p.h \
    places/qplaceuser_p.h

SOURCES += \
#data classes
    places/qplaceattribute.cpp \
    places/qplacecategory.cpp \
    places/qplacecontent.cpp \
    places/qplacecontentreply.cpp \
    places/qplaceeditorial.cpp \
    places/qplaceuser.cpp \
#result
    places/qplaceicon.cpp \
    places/qplaceimage.cpp \
    places/qplacerating.cpp \
    places/qplacereview.cpp \
    places/qplaceidreply.cpp \
    places/qplacesupplier.cpp \
#result
    places/qplacesearchresult.cpp \
#request classes
    places/qplacecontentrequest.cpp \
    places/qplacerequest.cpp \
    places/qplacesearchrequest.cpp \
#reply classes
    places/qplacereply.cpp \
    places/qplacedetailsreply.cpp \
    places/qplacesearchreply.cpp \
    places/qplacetextpredictionreply.cpp \
#manager and engine
    places/qplacemanager.cpp \
    places/qplacemanagerengine.cpp

