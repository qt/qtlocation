if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qgeotiledmap LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qgeotiledmap
    SOURCES
        tst_qgeotiledmap.cpp
    INCLUDE_DIRECTORIES
        ../geotestplugin
    LIBRARIES
        Qt::Core
        Qt::LocationPrivate
        Qt::PositioningPrivate
)
