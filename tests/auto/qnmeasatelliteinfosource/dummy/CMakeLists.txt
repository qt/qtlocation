# special case begin
qt_internal_add_test(tst_dummynmeasatelliteinfosource
    SOURCES
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../../utils/qnmeaproxyfactory.cpp ../../utils/qnmeaproxyfactory.h
        tst_dummynmeasatelliteinfosource.cpp
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
