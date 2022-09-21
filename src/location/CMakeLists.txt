qt_internal_add_module(Location
    PLUGIN_TYPES geoservices
    SOURCES
        qlocation.h qlocation.cpp
        qlocationglobal.h qlocationglobal_p.h
        maps/qgeoserviceprovider.h maps/qgeoserviceprovider_p.h maps/qgeoserviceprovider.cpp
        maps/qgeoserviceproviderfactory.h maps/qgeoserviceproviderfactory.cpp
        maps/qcache3q_p.h
        maps/qabstractgeotilecache_p.h maps/qabstractgeotilecache.cpp
        maps/qgeofiletilecache_p.h maps/qgeofiletilecache.cpp
        maps/qgeotilespec_p.h maps/qgeotilespec_p_p.h maps/qgeotilespec.cpp
        maps/qgeotiledmapscene_p.h maps/qgeotiledmapscene_p_p.h maps/qgeotiledmapscene.cpp
        maps/qgeotilerequestmanager_p.h maps/qgeotilerequestmanager.cpp
        maps/qgeotilefetcher_p.h maps/qgeotilefetcher_p_p.h maps/qgeotilefetcher.cpp
        maps/qgeotiledmap_p.h maps/qgeotiledmap_p_p.h maps/qgeotiledmap.cpp
        maps/qgeotiledmapreply_p.h maps/qgeotiledmapreply_p_p.h maps/qgeotiledmapreply.cpp
        maps/qgeotiledmappingmanagerengine_p.h maps/qgeotiledmappingmanagerengine_p_p.h
        maps/qgeotiledmappingmanagerengine.cpp
        maps/qgeocameradata_p.h maps/qgeocameradata.cpp
        maps/qgeocameracapabilities_p.h maps/qgeocameracapabilities.cpp
        maps/qgeocameratiles_p.h maps/qgeocameratiles_p_p.h maps/qgeocameratiles.cpp
        maps/qgeocodingmanagerengine.h maps/qgeocodingmanagerengine_p.h
        maps/qgeocodingmanagerengine.cpp
        maps/qgeocodingmanager.h maps/qgeocodingmanager_p.h maps/qgeocodingmanager.cpp
        maps/qgeocodereply.h maps/qgeocodereply_p.h maps/qgeocodereply.cpp
        maps/qgeoroutingmanager.h maps/qgeoroutingmanager_p.h maps/qgeoroutingmanager.cpp
        maps/qgeoroutingmanagerengine_p.h maps/qgeoroutingmanagerengine.h
        maps/qgeoroutingmanagerengine.cpp
        maps/qgeorouterequest.h maps/qgeorouterequest_p.h maps/qgeorouterequest.cpp
        maps/qgeoroutereply.h maps/qgeoroutereply_p.h maps/qgeoroutereply.cpp
        maps/qgeoroute.h maps/qgeoroute_p.h maps/qgeoroute.cpp
        maps/qgeoroutesegment.h maps/qgeoroutesegment_p.h maps/qgeoroutesegment.cpp
        maps/qgeorouteparser_p.h maps/qgeorouteparser_p_p.h maps/qgeorouteparser.cpp
        maps/qgeorouteparserosrmv4_p.h maps/qgeorouteparserosrmv4.cpp
        maps/qgeorouteparserosrmv5_p.h maps/qgeorouteparserosrmv5.cpp
        maps/qgeomaneuver.h maps/qgeomaneuver_p.h maps/qgeomaneuver.cpp
        maps/qgeomappingmanager_p.h maps/qgeomappingmanager_p_p.h maps/qgeomappingmanager.cpp
        maps/qgeomappingmanagerengine_p.h maps/qgeomappingmanagerengine_p_p.h
        maps/qgeomappingmanagerengine.cpp
        maps/qgeomaptype_p.h maps/qgeomaptype_p_p.h maps/qgeomaptype.cpp
        maps/qgeomap_p.h maps/qgeomap_p_p.h maps/qgeomap.cpp
        maps/qgeomapparameter_p.h maps/qgeomapparameter.cpp
        maps/qgeoprojection_p.h maps/qgeoprojection.cpp
        maps/qgeojson_p.h maps/qgeojson.cpp
        maps/qnavigationmanagerengine_p.h maps/qnavigationmanagerengine.cpp
        maps/qnavigationmanager_p.h maps/qnavigationmanager.cpp
        places/qplacemanager.h places/qplacemanager.cpp
        places/qplacemanagerengine.h places/qplacemanagerengine_p.h places/qplacemanagerengine.cpp
        places/unsupportedreplies_p.h
        places/qplacereply.h places/qplacereply_p.h places/qplacereply.cpp
        places/qplaceresult.h places/qplaceresult_p.h places/qplaceresult.cpp
        places/qplacesearchrequest.h places/qplacesearchrequest_p.h places/qplacesearchrequest.cpp
        places/qplacesearchreply.h places/qplacesearchreply.cpp
        places/qplacesearchresult.h places/qplacesearchresult_p.h places/qplacesearchresult.cpp
        places/qplaceproposedsearchresult.h places/qplaceproposedsearchresult_p.h
        places/qplaceproposedsearchresult.cpp
        places/qplacesearchsuggestionreply.h places/qplacesearchsuggestionreply.cpp
        places/qplacematchrequest.h places/qplacematchrequest.cpp
        places/qplacematchreply.h places/qplacematchreply.cpp
        places/qplacecontentrequest.h places/qplacecontentrequest_p.h
        places/qplacecontentrequest.cpp
        places/qplacecontentreply.h places/qplacecontentreply.cpp
        places/qplacedetailsreply.h places/qplacedetailsreply.cpp
        places/qplaceidreply.h places/qplaceidreply.cpp
        places/qplace.h places/qplace_p.h places/qplace.cpp
        places/qplaceicon.h places/qplaceicon_p.h places/qplaceicon.cpp
        places/qplacecategory.h places/qplacecategory_p.h places/qplacecategory.cpp
        places/qplaceuser_p.h places/qplaceuser.h places/qplaceuser.cpp
        places/qplacesupplier.h places/qplacesupplier_p.h places/qplacesupplier.cpp
        places/qplaceattribute.h places/qplaceattribute_p.h places/qplaceattribute.cpp
        places/qplaceratings.h places/qplaceratings_p.h places/qplaceratings.cpp
        places/qplacecontactdetail.h places/qplacecontactdetail_p.h places/qplacecontactdetail.cpp
        places/qplacecontent.h places/qplacecontent_p.h places/qplacecontent.cpp
        places/qplaceimage.h places/qplaceimage_p.h places/qplaceimage.cpp
        places/qplacereview.h places/qplacereview_p.h places/qplacereview.cpp
        places/qplaceeditorial.h places/qplaceeditorial_p.h places/qplaceeditorial.cpp
        declarativemaps/qdeclarativegeoserviceprovider_p.h
        declarativemaps/qdeclarativegeoserviceprovider.cpp
        declarativemaps/qdeclarativegeomaptype_p.h declarativemaps/qdeclarativegeomaptype.cpp
        declarativemaps/qdeclarativegeomapparameter_p.h
        declarativemaps/qdeclarativegeomapparameter.cpp
        declarativemaps/qparameterizableobject_p.h declarativemaps/qparameterizableobject.cpp
        declarativemaps/error_messages.cpp declarativemaps/error_messages_p.h
        declarativemaps/locationvaluetypehelper.cpp declarativemaps/locationvaluetypehelper_p.h
        declarativemaps/qdeclarativegeocodemodel.cpp declarativemaps/qdeclarativegeocodemodel_p.h
        declarativemaps/qdeclarativegeoroute.cpp declarativemaps/qdeclarativegeoroute_p.h
        declarativemaps/qdeclarativegeoroutemodel.cpp declarativemaps/qdeclarativegeoroutemodel_p.h
        quickmapitems/qgeomapitemgeometry.cpp quickmapitems/qgeomapitemgeometry_p.h
        quickmapitems/qdeclarativegeomap_p.h quickmapitems/qdeclarativegeomap.cpp
        quickmapitems/qdeclarativegeomapitembase_p.h
        quickmapitems/qdeclarativegeomapitembase.cpp
        quickmapitems/qdeclarativegeomapitemview_p.h
        quickmapitems/qdeclarativegeomapitemview.cpp
        quickmapitems/qgeosimplify.cpp quickmapitems/qgeosimplify_p.h
        quickmapitems/qdeclarativegeomapitemutils.cpp quickmapitems/qdeclarativegeomapitemutils_p.h
        quickmapitems/qdeclarativegeomapquickitem_p.h
        quickmapitems/qdeclarativegeomapquickitem.cpp
        quickmapitems/qdeclarativegeomapitemgroup_p.h
        quickmapitems/qdeclarativegeomapitemgroup.cpp
        quickmapitems/qdeclarativepolygonmapitem.cpp
        quickmapitems/qdeclarativepolygonmapitem_p.h
        quickmapitems/qdeclarativepolygonmapitem_p_p.h
        quickmapitems/qdeclarativepolylinemapitem.cpp
        quickmapitems/qdeclarativepolylinemapitem_p.h
        quickmapitems/qdeclarativepolylinemapitem_p_p.h
        quickmapitems/qdeclarativerectanglemapitem.cpp
        quickmapitems/qdeclarativerectanglemapitem_p.h
        quickmapitems/qdeclarativerectanglemapitem_p_p.h
        quickmapitems/qdeclarativecirclemapitem.cpp quickmapitems/qdeclarativecirclemapitem_p.h
        quickmapitems/qdeclarativecirclemapitem_p_p.h
        quickmapitems/qdeclarativeroutemapitem.cpp quickmapitems/qdeclarativeroutemapitem_p.h
        quickmapitems/qquickgeomapgesturearea_p.h quickmapitems/qquickgeomapgesturearea.cpp
        quickmapitems/qdeclarativegeomapcopyrightsnotice_p.h
        quickmapitems/qdeclarativegeomapcopyrightsnotice.cpp
        quickmapitems/qdeclarativegeomapitemtransitionmanager_p.h
        quickmapitems/qdeclarativegeomapitemtransitionmanager.cpp
        declarativemaps/qdeclarativegeoroutesegment.cpp
        declarativemaps/qdeclarativegeoroutesegment_p.h
        declarativeplaces/qdeclarativecategory.cpp
        declarativeplaces/qdeclarativecategory_p.h
        declarativeplaces/qdeclarativecontactdetail.cpp
        declarativeplaces/qdeclarativecontactdetail_p.h
        declarativeplaces/qdeclarativeplaceattribute.cpp
        declarativeplaces/qdeclarativeplaceattribute_p.h
        declarativeplaces/qdeclarativeplacecontentmodel.cpp
        declarativeplaces/qdeclarativeplacecontentmodel_p.h
        declarativeplaces/qdeclarativeplace.cpp
        declarativeplaces/qdeclarativeplaceeditorialmodel.cpp
        declarativeplaces/qdeclarativeplaceeditorialmodel_p.h
        declarativeplaces/qdeclarativeplaceicon.cpp
        declarativeplaces/qdeclarativeplaceicon_p.h
        declarativeplaces/qdeclarativeplaceimagemodel.cpp
        declarativeplaces/qdeclarativeplaceimagemodel_p.h
        declarativeplaces/qdeclarativeplace_p.h
        declarativeplaces/qdeclarativeplaceuser.cpp
        declarativeplaces/qdeclarativeplaceuser_p.h
        declarativeplaces/qdeclarativeratings.cpp
        declarativeplaces/qdeclarativeratings_p.h
        declarativeplaces/qdeclarativereviewmodel.cpp
        declarativeplaces/qdeclarativereviewmodel_p.h
        declarativeplaces/qdeclarativesearchmodelbase.cpp
        declarativeplaces/qdeclarativesearchmodelbase_p.h
        declarativeplaces/qdeclarativesearchresultmodel.cpp
        declarativeplaces/qdeclarativesearchresultmodel_p.h
        declarativeplaces/qdeclarativesearchsuggestionmodel.cpp
        declarativeplaces/qdeclarativesearchsuggestionmodel_p.h
        declarativeplaces/qdeclarativesupplier.cpp
        declarativeplaces/qdeclarativesupplier_p.h
        declarativeplaces/qdeclarativesupportedcategoriesmodel.cpp
        declarativeplaces/qdeclarativesupportedcategoriesmodel_p.h
    INCLUDE_DIRECTORIES
        maps
        places
        declarativemaps
        declarativeplaces
        quickmapitems
        ../3rdparty/earcut
    LIBRARIES
        Qt::CorePrivate
        Qt::Gui
        Qt::QuickPrivate
        Qt::PositioningPrivate
        Qt::PositioningQuickPrivate
        Qt::Qml
        Qt::QmlModelsPrivate
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
        Qt::QmlPrivate
        Qt::QmlModelsPrivate
    DEFINES
        QT_NO_FOREACH
)

qt_internal_extend_target(Location CONDITION QT_FEATURE_location_labs_plugin
    SOURCES
        labs/qdeclarativenavigator.cpp labs/qdeclarativenavigator_p.h labs/qdeclarativenavigator_p_p.h
    INCLUDE_DIRECTORIES
        labs
)

qt_internal_add_shaders(Location "declarative_location_shaders"
    BATCHABLE
    PRECOMPILE
    OPTIMIZED
    PREFIX
        "/location"
    FILES
        "quickmapitems/shaders/polyline_linestrip.vert"
        "quickmapitems/shaders/polyline_linestrip.frag"
        "quickmapitems/shaders/polyline_extruded.vert"
        "quickmapitems/shaders/polyline_extruded.frag"
        "quickmapitems/shaders/polygon.vert"
        "quickmapitems/shaders/polygon.frag"
)

qt_internal_add_docs(Location
    doc/qtlocation.qdocconf
)
