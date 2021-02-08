# special case begin
qt_internal_add_test(tst_nmeasatelliteinfosourcegeneric
    SOURCES
        ../../qgeosatelliteinfosource/testqgeosatelliteinfosource.cpp ../../qgeosatelliteinfosource/testqgeosatelliteinfosource_p.h
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../../utils/qnmeaproxyfactory.cpp ../../utils/qnmeaproxyfactory.h
        tst_nmeasatelliteinfosourcegeneric.cpp
    DEFINES
        QT_DISABLE_DEPRECATED_BEFORE=0
    INCLUDE_DIRECTORIES
        ..
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Network
        Qt::Positioning
)
# special case end
