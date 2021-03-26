# Generated from qnmeapositioninfosource_simulation_generic.pro.

#####################################################################
## tst_qnmeapositioninfosource_simulation_generic Test:
#####################################################################

# special case begin

# Renamed the target due to problems with the maximum full path on
# Windows systems (which is 250 characters only)
qt_internal_add_test(tst_qnmeaposinfosource_sim_generic
# special case end
    SOURCES
        ../../qgeopositioninfosource/testqgeopositioninfosource.cpp ../../qgeopositioninfosource/testqgeopositioninfosource_p.h
        ../../utils/qlocationtestutils.cpp ../../utils/qlocationtestutils_p.h
        ../../utils/qnmeaproxyfactory.cpp ../../utils/qnmeaproxyfactory.h
        ../tst_qnmeapositioninfosource.cpp ../tst_qnmeapositioninfosource.h
        tst_qnmeapositioninfosource_simulation_generic.cpp
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

#### Keys ignored in scope 1:.:.:qnmeapositioninfosource_simulation_generic.pro:<TRUE>:
# TEMPLATE = "app"
# testcase.timeout = "400"
