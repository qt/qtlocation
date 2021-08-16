# Generated from logfilepositionsource.pro.

cmake_minimum_required(VERSION 3.16)
project(logfilepositionsource LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/positioning/logfilepositionsource")

find_package(Qt6 COMPONENTS Positioning)
find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Widgets)

qt_add_executable(logfilepositionsource
    clientapplication.cpp clientapplication.h
    logfilepositionsource.cpp logfilepositionsource.h
    main.cpp
)
set_target_properties(logfilepositionsource PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(logfilepositionsource PRIVATE
    Qt::Core
    Qt::Positioning
    Qt::Widgets
)


# Resources:
set(logfile_resource_files
    "simplelog.txt"
)

qt6_add_resources(logfilepositionsource "logfile"
    PREFIX
        "/"
    FILES
        ${logfile_resource_files}
)

install(TARGETS logfilepositionsource
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
