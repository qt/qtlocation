# special case begin

qt_internal_add_benchmark(tst_bench_qgeopositioninfo
    SOURCES
        tst_bench_qgeopositioninfo.cpp
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Positioning
        Qt::Test
)

# special case end
