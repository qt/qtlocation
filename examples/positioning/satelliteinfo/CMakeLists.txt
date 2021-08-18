# Generated from satelliteinfo.pro.

cmake_minimum_required(VERSION 3.16)
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

qt_add_qml_module(satelliteinfo
    URI Local
    VERSION 1.0
    QML_FILES satelliteinfo.qml
    NO_RESOURCE_TARGET_PATH
)

install(TARGETS satelliteinfo
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
