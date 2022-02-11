qt_internal_add_module(Location
    PLUGIN_TYPES geoservices
    SOURCES
        qlocation.h qlocation.cpp qlocationglobal_p.h qlocationglobal.h
        maps/qgeotiledmapscene.cpp maps/qgeotilerequestmanager.cpp
        maps/qgeoroute.cpp maps/qgeoroutesegment.cpp maps/qgeocameradata.cpp
        maps/qgeotilespec.cpp maps/qgeomappingmanagerengine.cpp
        maps/qnavigationmanagerengine.cpp maps/qgeocameratiles.cpp
        maps/qgeotilefetcher.cpp maps/qgeocodingmanagerengine.cpp
        maps/qgeocodingmanager.cpp maps/qgeorouteparserosrmv5.cpp
        maps/qgeoroutingmanager.cpp maps/qgeofiletilecache.cpp
        maps/qgeoserviceproviderfactory.cpp maps/qgeomapparameter.cpp
        maps/qgeomappingmanager.cpp maps/qgeotiledmap.cpp
        maps/qgeoprojection.cpp maps/qnavigationmanager.cpp
        maps/qgeotiledmapreply.cpp maps/qgeotiledmappingmanagerengine.cpp
        maps/qgeomaptype.cpp maps/qgeoserviceprovider.cpp
        maps/qgeoroutereply.cpp maps/qgeorouteparser.cpp
        maps/qgeorouterequest.cpp maps/qgeomap.cpp
        maps/qgeoroutingmanagerengine.cpp maps/qgeorouteparserosrmv4.cpp
        maps/qgeomaneuver.cpp maps/qgeocameracapabilities.cpp
        maps/qabstractgeotilecache.cpp maps/qgeocodereply.cpp
        maps/qgeotilerequestmanager_p.h
        maps/qgeomaneuver.h maps/qgeotiledmapscene_p_p.h
        maps/qgeoroutingmanagerengine.h maps/qgeorouterequest.h
        maps/qgeocodingmanagerengine.h maps/qgeotiledmapreply_p.h
        maps/qgeorouteparser_p.h maps/qgeocameracapabilities_p.h
        maps/qgeocameratiles_p.h maps/qgeoroutesegment_p.h
        maps/qgeocodingmanager_p.h maps/qgeoprojection_p.h
        maps/qgeotiledmapscene_p.h maps/qgeotilefetcher_p_p.h
        maps/qgeocodereply_p.h maps/qgeorouteparserosrmv5_p.h
        maps/qgeorouterequest_p.h maps/qgeocodingmanagerengine_p.h
        maps/qgeomaptype_p.h maps/qgeorouteparserosrmv4_p.h
        maps/qgeotilespec_p_p.h maps/qgeocodingmanager.h
        maps/qgeoroutesegment.h maps/qgeoroutingmanager.h
        maps/qgeotiledmappingmanagerengine_p.h maps/qgeotilefetcher_p.h
        maps/qgeomaptype_p_p.h maps/qgeocameratiles_p_p.h
        maps/qgeoserviceprovider.h maps/qgeotiledmap_p_p.h
        maps/qgeorouteparser_p_p.h maps/qgeomaneuver_p.h
        maps/qgeocameradata_p.h maps/qgeomappingmanagerengine_p.h
        maps/qabstractgeotilecache_p.h maps/qgeofiletilecache_p.h
        maps/qcache3q_p.h maps/qgeomappingmanagerengine_p_p.h
        maps/qgeotilespec_p.h maps/qgeomappingmanager_p_p.h
        maps/qgeotiledmap_p.h maps/qnavigationmanagerengine_p.h
        maps/qgeoroute.h maps/qgeoroutingmanager_p.h
        maps/qgeomapparameter_p.h maps/qgeoroutereply.h
        maps/qgeoroute_p.h maps/qgeoserviceproviderfactory.h
        maps/qgeomappingmanager_p.h maps/qgeotiledmappingmanagerengine_p_p.h
        maps/qgeoserviceprovider_p.h maps/qnavigationmanager_p.h
        maps/qgeotiledmapreply_p_p.h maps/qgeoroutereply_p.h
        maps/qgeocodereply.h maps/qgeomap_p_p.h
        maps/qgeoroutingmanagerengine_p.h maps/qgeomap_p.h
        places/qplacesearchrequest.cpp places/qplaceicon.cpp
        places/qplacematchrequest.cpp places/qplacemanager.cpp
        places/qplacecategory.cpp places/qplaceuser.cpp
        places/qplacesearchresult.cpp places/qplacemanagerengine.cpp
        places/qplacereview.cpp places/qplacecontentreply.cpp
        places/qplacecontentrequest.cpp places/qplacesearchreply.cpp
        places/qplacereply.cpp places/qplacedetailsreply.cpp
        places/qplaceratings.cpp places/qplacecontactdetail.cpp
        places/qplaceproposedsearchresult.cpp places/qplaceeditorial.cpp
        places/qplace.cpp places/qplacesupplier.cpp
        places/qplaceattribute.cpp places/qplaceimage.cpp
        places/qplacesearchsuggestionreply.cpp places/qplaceresult.cpp
        places/qplacematchreply.cpp places/qplacecontent.cpp
        places/qplaceidreply.cpp
        places/placemacro.h places/qplaceicon.h places/qplaceuser.h
        places/qplaceattribute_p.h places/qplacematchrequest.h
        places/qplacecontentreply.h places/qplacereview.h
        places/qplacecategory.h places/qplacesearchrequest.h
        places/qplacecategory_p.h places/qplaceeditorial_p.h
        places/qplaceproposedsearchresult.h places/qplacemanagerengine.h
        places/qplacecontentrequest.h places/qplacemanagerengine_p.h
        places/qplace.h places/qplacecontentrequest_p.h
        places/qplaceeditorial.h places/qplaceattribute.h
        places/qplacedetailsreply.h places/qplaceimage_p.h
        places/qplaceratings_p.h places/unsupportedreplies_p.h
        places/qplacereply.h places/qplace_p.h places/qplaceresult_p.h
        places/qplacemanager.h places/qplacereply_p.h
        places/qplacesearchresult_p.h places/qplaceresult.h
        places/qplacereview_p.h places/qplacematchreply.h
        places/qplacecontent.h places/qplacesearchreply.h
        places/qplacesupplier_p.h places/qplaceratings.h
        places/qplaceimage.h places/qplacesearchresult.h
        places/qplaceidreply.h places/qplaceuser_p.h places/qplaceicon_p.h
        places/qplacecontactdetail.h places/qplacesearchsuggestionreply.h
        places/qplacecontent_p.h places/qplacesearchrequest_p.h
        places/qplaceproposedsearchresult_p.h places/qplacesupplier.h
        places/qplacecontactdetail_p.h
        declarativemaps/qdeclarativegeomapitembase_p.h
        declarativemaps/qdeclarativegeomapitembase.cpp
        declarativemaps/qdeclarativegeomap_p.h declarativemaps/qdeclarativegeomap.cpp
        declarativemaps/qdeclarativegeomapitemtransitionmanager_p.h
        declarativemaps/qdeclarativegeomapitemtransitionmanager.cpp
        declarativemaps/qquickgeomapgesturearea_p.h declarativemaps/qquickgeomapgesturearea.cpp
        declarativemaps/qdeclarativegeomapitemgroup_p.h
        declarativemaps/qdeclarativegeomapitemgroup.cpp
        declarativemaps/qdeclarativegeomapquickitem_p.h
        declarativemaps/qdeclarativegeomapquickitem.cpp
        declarativemaps/qdeclarativegeomapcopyrightsnotice_p.h
        declarativemaps/qdeclarativegeomapcopyrightsnotice.cpp
        declarativemaps/qdeclarativegeoserviceprovider_p.h
        declarativemaps/qdeclarativegeoserviceprovider.cpp
        declarativemaps/qdeclarativegeomaptype_p.h declarativemaps/qdeclarativegeomaptype.cpp
        declarativemaps/qdeclarativegeomapparameter_p.h
        declarativemaps/qdeclarativegeomapparameter.cpp
        declarativemaps/qdeclarativegeomapitemview_p.h
        declarativemaps/qdeclarativegeomapitemview.cpp
        declarativemaps/qgeomapobject_p.h declarativemaps/qgeomapobject_p_p.h
        declarativemaps/qgeomapobject.cpp
        declarativemaps/qparameterizableobject_p.h declarativemaps/qparameterizableobject.cpp
        labs/qgeojson_p.h labs/qgeojson.cpp
    INCLUDE_DIRECTORIES
        maps
        places
        declarativemaps
    LIBRARIES
        Qt::CorePrivate
        Qt::Gui
        Qt::QuickPrivate
        Qt::PositioningPrivate
        Qt::PositioningQuickPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
        Qt::Network
        Qt::PositioningQuick
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::QuickPrivate
        Qt::PositioningPrivate
        Qt::PositioningQuickPrivate
)

# TODO - add QML stuff
# TODO - add docs
