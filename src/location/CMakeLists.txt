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
        places/qplacecontentreply.cpp
        places/qplacecontentrequest.cpp places/qplacesearchreply.cpp
        places/qplacereply.cpp places/qplacedetailsreply.cpp
        places/qplaceratings.cpp places/qplacecontactdetail.cpp
        places/qplaceproposedsearchresult.cpp
        places/qplace.cpp places/qplacesupplier.cpp
        places/qplaceattribute.cpp
        places/qplacesearchsuggestionreply.cpp places/qplaceresult.cpp
        places/qplacematchreply.cpp places/qplacecontent.cpp
        places/qplaceidreply.cpp
        places/qplaceicon.h places/qplaceuser.h
        places/qplaceattribute_p.h places/qplacematchrequest.h
        places/qplacecontentreply.h
        places/qplacecategory.h places/qplacesearchrequest.h
        places/qplacecategory_p.h
        places/qplaceproposedsearchresult.h places/qplacemanagerengine.h
        places/qplacecontentrequest.h places/qplacemanagerengine_p.h
        places/qplace.h places/qplacecontentrequest_p.h
        places/qplaceattribute.h
        places/qplacedetailsreply.h
        places/qplaceratings_p.h places/unsupportedreplies_p.h
        places/qplacereply.h places/qplace_p.h places/qplaceresult_p.h
        places/qplacemanager.h places/qplacereply_p.h
        places/qplacesearchresult_p.h places/qplaceresult.h
        places/qplacematchreply.h
        places/qplacecontent.h places/qplacesearchreply.h
        places/qplacesupplier_p.h places/qplaceratings.h
        places/qplacesearchresult.h
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
        declarativemaps/qdeclarativegeomapparameter_p.h
        declarativemaps/qdeclarativegeomapparameter.cpp
        declarativemaps/qdeclarativegeomapitemview_p.h
        declarativemaps/qdeclarativegeomapitemview.cpp
        declarativemaps/qgeomapobject_p.h declarativemaps/qgeomapobject_p_p.h
        declarativemaps/qgeomapobject.cpp
        declarativemaps/qparameterizableobject_p.h declarativemaps/qparameterizableobject.cpp
        declarativemaps/qdeclarativegeomapitemutils.cpp
        declarativemaps/qdeclarativegeomapitemutils_p.h
        declarativemaps/qdeclarativepolylinemapitem.cpp
        declarativemaps/qdeclarativepolylinemapitem_p.h
        declarativemaps/qdeclarativepolylinemapitem_p_p.h
        declarativemaps/error_messages.cpp declarativemaps/error_messages_p.h
        declarativemaps/qdeclarativerectanglemapitem.cpp
        declarativemaps/qdeclarativerectanglemapitem_p.h
        declarativemaps/qdeclarativerectanglemapitem_p_p.h
        declarativemaps/qdeclarativecirclemapitem.cpp declarativemaps/qdeclarativecirclemapitem_p.h
        declarativemaps/qdeclarativecirclemapitem_p_p.h
        declarativemaps/qdeclarativeroutemapitem.cpp declarativemaps/qdeclarativeroutemapitem_p.h
        declarativemaps/qdeclarativegeocodemodel.cpp declarativemaps/qdeclarativegeocodemodel_p.h
        declarativemaps/qgeomapitemgeometry.cpp declarativemaps/qgeomapitemgeometry_p.h
        declarativemaps/qdeclarativepolygonmapitem.cpp
        declarativemaps/qdeclarativepolygonmapitem_p.h
        declarativemaps/qdeclarativepolygonmapitem_p_p.h
        declarativemaps/qdeclarativegeoroute.cpp declarativemaps/qdeclarativegeoroute_p.h
        declarativemaps/qdeclarativegeoroutemodel.cpp declarativemaps/qdeclarativegeoroutemodel_p.h
        declarativemaps/qgeosimplify.cpp declarativemaps/qgeosimplify_p.h
        declarativeplaces/qdeclarativecategory.cpp
        declarativeplaces/qdeclarativecategory_p.h
        declarativeplaces/qdeclarativecontactdetails.cpp
        declarativeplaces/qdeclarativecontactdetails_p.h
        declarativeplaces/qdeclarativeplacecontentmodel.cpp
        declarativeplaces/qdeclarativeplacecontentmodel_p.h
        declarativeplaces/qdeclarativeplace.cpp
        declarativeplaces/qdeclarativeplace_p.h
        declarativeplaces/qdeclarativesearchmodelbase.cpp
        declarativeplaces/qdeclarativesearchmodelbase_p.h
        declarativeplaces/qdeclarativesearchresultmodel.cpp
        declarativeplaces/qdeclarativesearchresultmodel_p.h
        declarativeplaces/qdeclarativesearchsuggestionmodel.cpp
        declarativeplaces/qdeclarativesearchsuggestionmodel_p.h
        declarativeplaces/qdeclarativesupportedcategoriesmodel.cpp
        declarativeplaces/qdeclarativesupportedcategoriesmodel_p.h
        labs/qgeojson_p.h labs/qgeojson.cpp
    INCLUDE_DIRECTORIES
        maps
        places
        declarativemaps
        declarativeplaces
        ../3rdparty/earcut
    LIBRARIES
        Qt::CorePrivate
        Qt::Gui
        Qt::QuickPrivate
        Qt::PositioningPrivate
        Qt::PositioningQuickPrivate
        Qt::Qml
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
    DEFINES
        QT_NO_FOREACH
    GENERATE_CPP_EXPORTS
    GENERATE_PRIVATE_CPP_EXPORTS
)

qt_internal_extend_target(Location CONDITION QT_FEATURE_location_labs_plugin
    SOURCES
        labs/qdeclarativenavigator.cpp labs/qdeclarativenavigator_p.h labs/qdeclarativenavigator_p_p.h
#        labs/qgeojson.cpp labs/qgeojson_p.h
        labs/qgeotiledmaplabs.cpp labs/qgeotiledmaplabs_p.h
        labs/qmapcircleobject.cpp labs/qmapcircleobject_p.h labs/qmapcircleobject_p_p.h
        labs/qmapiconobject.cpp labs/qmapiconobject_p.h labs/qmapiconobject_p_p.h
        labs/qmapobjectview.cpp labs/qmapobjectview_p.h labs/qmapobjectview_p_p.h
        labs/qmappolygonobject.cpp labs/qmappolygonobject_p.h labs/qmappolygonobject_p_p.h
        labs/qmappolylineobject.cpp labs/qmappolylineobject_p.h labs/qmappolylineobject_p_p.h
        labs/qmaprouteobject.cpp labs/qmaprouteobject_p.h labs/qmaprouteobject_p_p.h
        labs/qsg/qgeomapobjectqsgsupport.cpp labs/qsg/qgeomapobjectqsgsupport_p.h
        labs/qsg/qmapcircleobjectqsg.cpp labs/qsg/qmapcircleobjectqsg_p_p.h
        labs/qsg/qmapiconobjectqsg.cpp labs/qsg/qmapiconobjectqsg_p_p.h
        labs/qsg/qmappolygonobjectqsg.cpp labs/qsg/qmappolygonobjectqsg_p_p.h
        labs/qsg/qmappolylineobjectqsg.cpp labs/qsg/qmappolylineobjectqsg_p_p.h
        labs/qsg/qmaprouteobjectqsg.cpp labs/qsg/qmaprouteobjectqsg_p_p.h
        labs/qsg/qqsgmapobject.cpp labs/qsg/qqsgmapobject_p.h
    INCLUDE_DIRECTORIES
        labs
)

qt_internal_add_shaders(Location "declarative_location_shaders"
    BATCHABLE
    PRECOMPILE
    OPTIMIZED
    PREFIX
        "/location/declarativemaps"
    FILES
        "declarativemaps/shaders/polyline_linestrip.vert"
        "declarativemaps/shaders/polyline_linestrip.frag"
        "declarativemaps/shaders/polyline_extruded.vert"
        "declarativemaps/shaders/polyline_extruded.frag"
        "declarativemaps/shaders/polygon.vert"
        "declarativemaps/shaders/polygon.frag"
)

qt_internal_add_docs(Location
    doc/qtlocation.qdocconf
)
