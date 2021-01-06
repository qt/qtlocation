# Generated from dummynmeapositioninfosource.pro.

#####################################################################
## tst_dummynmeapositioninfosource Test:
#####################################################################

qt_add_test(tst_dummynmeapositioninfosource
    SOURCES
        ../../qgeopositioninfosource/testqgeopositioninfosource.cpp ../../qgeopositioninfosource/testqgeopositioninfosource_p.h
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../qnmeapositioninfosourceproxyfactory.cpp ../qnmeapositioninfosourceproxyfactory.h
        tst_dummynmeapositioninfosource.cpp
    DEFINES
        QT_DISABLE_DEPRECATED_BEFORE=0
    INCLUDE_DIRECTORIES
        ..
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Network
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:dummynmeapositioninfosource.pro:<TRUE>:
# TEMPLATE = "app"
