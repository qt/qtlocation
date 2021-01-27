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

# special case begin

# These properties are required for the qt6_qml_type_registration()
# to work properly
set_target_properties(tst_declarative_positioning_core
    PROPERTIES
    QT_QML_MODULE_URI TestFactory
    QT_QML_MODULE_VERSION 1.0
)

qt6_qml_type_registration(tst_declarative_positioning_core)

# special case end

#### Keys ignored in scope 1:.:.:declarative_positioning_core.pro:<TRUE>:
# TEMPLATE = "app"
