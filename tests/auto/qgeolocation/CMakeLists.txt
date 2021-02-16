# Generated from qgeolocation.pro.

#####################################################################
## tst_qgeolocation Test:
#####################################################################

qt_internal_add_test(tst_qgeolocation
    SOURCES
        ../utils/qlocationtestutils.cpp ../utils/qlocationtestutils_p.h
        tst_qgeolocation.cpp tst_qgeolocation.h
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
)
