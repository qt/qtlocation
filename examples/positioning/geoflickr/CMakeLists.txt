# Generated from geoflickr.pro.

cmake_minimum_required(VERSION 3.16)
project(geoflickr LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/positioning/geoflickr")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Network)
find_package(Qt6 COMPONENTS Positioning)

qt_add_executable(geoflickr
    qmllocationflickr.cpp
)
set_target_properties(geoflickr PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(geoflickr PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::Positioning
    Qt::Qml
    Qt::Quick
)


# Resources:
set(flickr_resource_files
    "flickr-90.qml"
    "flickr.qml"
    "flickrcommon/Progress.qml"
    "flickrcommon/RestModel.qml"
    "flickrcommon/ScrollBar.qml"
    "flickrcommon/Slider.qml"
    "flickrmobile/Button.qml"
    "flickrmobile/GeoTab.qml"
    "flickrmobile/GridDelegate.qml"
    "flickrmobile/ImageDetails.qml"
    "flickrmobile/ListDelegate.qml"
    "flickrmobile/TitleBar.qml"
    "flickrmobile/ToolBar.qml"
    "flickrmobile/images/gloss.png"
    "flickrmobile/images/lineedit.png"
    "flickrmobile/images/lineedit.sci"
    "flickrmobile/images/moon.png"
    "flickrmobile/images/quit.png"
    "flickrmobile/images/star.png"
    "flickrmobile/images/stripes.png"
    "flickrmobile/images/sun.png"
    "flickrmobile/images/titlebar.png"
    "flickrmobile/images/titlebar.sci"
    "flickrmobile/images/toolbutton.png"
    "flickrmobile/images/toolbutton.sci"
    "flickrmobile/nmealog.txt"
)

qt6_add_resources(geoflickr "flickr"
    PREFIX
        "/"
    FILES
        ${flickr_resource_files}
)

install(TARGETS geoflickr
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
