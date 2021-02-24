# Generated from positioning.pro.

if(TARGET Qt::Widgets)
    add_subdirectory(logfilepositionsource)
endif()
if(TARGET Qt::Quick)
    add_subdirectory(satelliteinfo)
endif()
if(TARGET Qt::Quick)
    add_subdirectory(geoflickr)
endif()
if(TARGET Qt::Network AND TARGET Qt::Quick)
    add_subdirectory(weatherinfo)
endif()
