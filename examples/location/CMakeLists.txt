# Generated from location.pro.

if(TARGET Qt::Quick)
    add_subdirectory(places)
    add_subdirectory(places_list)
    add_subdirectory(places_map)
#    add_subdirectory(mapviewer)
    add_subdirectory(minimal_map)
    add_subdirectory(itemview_transitions)
    add_subdirectory(planespotter)
endif()
#if(TARGET Qt::Quick AND TARGET Qt::Widgets)
#    add_subdirectory(geojson_viewer)
#endif()
