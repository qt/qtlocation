# Generated from qnmeapositioninfosource_realtime.pro.

#####################################################################
## tst_qnmeapositioninfosource_realtime Test:
#####################################################################

qt_internal_add_test(tst_qnmeapositioninfosource_realtime
    SOURCES
        ../../qgeopositioninfosource/testqgeopositioninfosource.cpp ../../qgeopositioninfosource/testqgeopositioninfosource_p.h
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../../utils/qnmeaproxyfactory.cpp ../../utils/qnmeaproxyfactory.h
        ../tst_qnmeapositioninfosource.cpp ../tst_qnmeapositioninfosource.h
        tst_qnmeapositioninfosource_realtime.cpp
    DEFINES
        QT_DISABLE_DEPRECATED_BEFORE=0
    INCLUDE_DIRECTORIES
        ..
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Network
        Qt::Positioning
)

#### Keys ignored in scope 1:.:.:qnmeapositioninfosource_realtime.pro:<TRUE>:
# TEMPLATE = "app"
