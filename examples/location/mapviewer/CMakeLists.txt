# Generated from mapviewer.pro.

cmake_minimum_required(VERSION 3.16)
project(qml_location_mapviewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/location/mapviewer")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Network)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Positioning)
find_package(Qt6 COMPONENTS Location)

qt_add_executable(qml_location_mapviewer WIN32 MACOSX_BUNDLE
    main.cpp
)
target_link_libraries(qml_location_mapviewer PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Location
    Qt::Network
    Qt::Positioning
    Qt::Qml
    Qt::Quick
)

# Resources:
set(mapviewer_resource_files
    "forms/Geocode.qml"
    "forms/GeocodeForm.ui.qml"
    "forms/Locale.qml"
    "forms/LocaleForm.ui.qml"
    "forms/Message.qml"
    "forms/MessageForm.ui.qml"
    "forms/ReverseGeocode.qml"
    "forms/ReverseGeocodeForm.ui.qml"
    "forms/RouteAddress.qml"
    "forms/RouteAddressForm.ui.qml"
    "forms/RouteCoordinate.qml"
    "forms/RouteCoordinateForm.ui.qml"
    "forms/RouteList.qml"
    "forms/RouteListDelegate.qml"
    "forms/RouteListHeader.qml"
    "helper.js"
    "map/MapComponent.qml"
    "map/MapSliders.qml"
    "map/Marker.qml"
    "map/MiniMap.qml"
    "mapviewer.qml"
    "menus/ItemPopupMenu.qml"
    "menus/MainMenu.qml"
    "menus/MapPopupMenu.qml"
    "menus/MarkerPopupMenu.qml"
    "resources/marker.png"
    "resources/marker_blue.png"
    "resources/scale.png"
    "resources/scale_end.png"
)

qt6_add_resources(qml_location_mapviewer "mapviewer"
    PREFIX
        "/"
    FILES
        ${mapviewer_resource_files}
)

if(QT_FEATURE_geoservices_maplibregl)
    target_link_libraries(qml_location_mapviewer PUBLIC
        Qt::Sql
    )
endif()

install(TARGETS qml_location_mapviewer
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
