if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qgeoroutereply LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

qt_internal_add_test(tst_qgeoroutereply
    SOURCES
        tst_qgeoroutereply.h tst_qgeoroutereply.cpp
    LIBRARIES
        Qt::Core
        Qt::Location
        Qt::Positioning
)
