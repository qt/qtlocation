# Generated from qgeocoordinate.pro.

#####################################################################
## tst_qgeocoordinate Test:
#####################################################################

qt_internal_add_test(tst_qgeocoordinate
    SOURCES
        ../utils/qlocationtestutils.cpp ../utils/qlocationtestutils_p.h
        tst_qgeocoordinate.cpp
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
)
