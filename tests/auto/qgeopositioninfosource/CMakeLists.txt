# Generated from qgeopositioninfosource.pro.

#####################################################################
## tst_qgeopositioninfosource Test:
#####################################################################

qt_internal_add_test(tst_qgeopositioninfosource
    SOURCES
        ../utils/qlocationtestutils.cpp ../utils/qlocationtestutils_p.h
        testqgeopositioninfosource.cpp testqgeopositioninfosource_p.h
        tst_qgeopositioninfosource.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:qgeopositioninfosource.pro:<TRUE>:
# TEMPLATE = "app"
# testcase.timeout = "400"
