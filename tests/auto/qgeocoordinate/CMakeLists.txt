# Generated from qgeocoordinate.pro.

#####################################################################
## tst_qgeocoordinate Test:
#####################################################################

qt_add_test(tst_qgeocoordinate
    SOURCES
        ../utils/qlocationtestutils.cpp ../utils/qlocationtestutils_p.h
        tst_qgeocoordinate.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Positioning
)
