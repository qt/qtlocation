# special case begin
qt_internal_add_test(tst_nmeasatelliteinfosource
    SOURCES
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        tst_nmeasatelliteinfosource.cpp
    DEFINES
        QT_DISABLE_DEPRECATED_BEFORE=0
    INCLUDE_DIRECTORIES
        ..
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
)
# special case end
