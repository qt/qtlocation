# Generated from satelliteinfo.pro.

cmake_minimum_required(VERSION 3.14)
project(satelliteinfo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/positioning/satelliteinfo")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Positioning)

qt_add_executable(satelliteinfo
    main.cpp
    satellitemodel.cpp satellitemodel.h
)
set_target_properties(satelliteinfo PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(satelliteinfo PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Positioning
    Qt::Quick
)


# Resources:
set(satelliteinfo_resource_files
    "satelliteinfo.qml"
)

qt6_add_resources(satelliteinfo "satelliteinfo"
    PREFIX
        "/"
    FILES
        ${satelliteinfo_resource_files}
)

install(TARGETS satelliteinfo
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

set_target_properties(satelliteinfo PROPERTIES
    QT_QML_MODULE_VERSION 1.0
    QT_QML_MODULE_URI Local
)
qt6_qml_type_registration(satelliteinfo)
