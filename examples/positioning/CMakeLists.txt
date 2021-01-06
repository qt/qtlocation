# Generated from positioning.pro.

if(TARGET Qt::Widgets)
    add_subdirectory(logfilepositionsource)
endif()
if(TARGET Qt::Quick)
    add_subdirectory(satelliteinfo)
endif()
# TODO: enable when the XmlListModel is ported
# if(TARGET Qt::Quick AND TARGET Qt::XmlPatterns)
#     add_subdirectory(geoflickr)
# endif()
if(TARGET Qt::Network AND TARGET Qt::Quick)
    add_subdirectory(weatherinfo)
endif()
