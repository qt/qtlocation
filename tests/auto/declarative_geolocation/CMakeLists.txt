# special case begin

# Collect test data

qt_internal_add_test(tst_declarative_geolocation
    QMLTEST
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::Positioning
        Qt::Quick
    TESTDATA
        tst_declarativegeolocation.qml
)

# special case end
