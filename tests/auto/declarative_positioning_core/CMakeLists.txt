# Generated from declarative_positioning_core.pro.

#####################################################################
## tst_declarative_positioning_core Test:
#####################################################################

# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.qml)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_declarative_positioning_core
    QMLTEST
    SOURCES
        main.cpp
        factory.cpp
        factory.h
    PUBLIC_LIBRARIES
        Qt::Positioning
        Qt::Quick
    TESTDATA ${test_data}
)

qt_add_qml_module(tst_declarative_positioning_core
    URI TestFactory
    VERSION 1.0
    QML_FILES
        tst_address.qml
        tst_coordinate.qml
        tst_geoshape.qml
        tst_position.qml
        tst_positionsource.qml
    NO_RESOURCE_TARGET_PATH
)
