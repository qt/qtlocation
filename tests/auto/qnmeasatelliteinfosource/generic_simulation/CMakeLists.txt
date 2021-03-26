# special case begin
qt_internal_add_test(tst_nmeasatelliteinfosource_generic_sim
    SOURCES
        ../../qgeosatelliteinfosource/testqgeosatelliteinfosource.cpp ../../qgeosatelliteinfosource/testqgeosatelliteinfosource_p.h
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../../utils/qnmeaproxyfactory.cpp ../../utils/qnmeaproxyfactory.h
        tst_nmeasatelliteinfosource_generic_sim.cpp
    DEFINES
        QT_DISABLE_DEPRECATED_BEFORE=0
    INCLUDE_DIRECTORIES
        ..
    LIBRARIES
        Qt::Core
        Qt::Network
        Qt::Positioning
        Qt::TestPrivate
)
# special case end
