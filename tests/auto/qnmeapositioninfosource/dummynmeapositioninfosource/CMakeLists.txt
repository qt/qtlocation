# Generated from dummynmeapositioninfosource.pro.

#####################################################################
## tst_dummynmeapositioninfosource Test:
#####################################################################

qt_internal_add_test(tst_dummynmeapositioninfosource
    SOURCES
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../../utils/qnmeaproxyfactory.cpp ../../utils/qnmeaproxyfactory.h
        tst_dummynmeapositioninfosource.cpp
    DEFINES
        QT_DISABLE_DEPRECATED_BEFORE=0
    INCLUDE_DIRECTORIES
        ..
    LIBRARIES
        Qt::Core
        Qt::Network
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:dummynmeapositioninfosource.pro:<TRUE>:
# TEMPLATE = "app"
