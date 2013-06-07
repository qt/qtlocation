TEMPLATE = lib
TARGET = poly2tri

CONFIG += staticlib
CONFIG -= qt

contains(QT_CONFIG, debug_and_release):CONFIG += debug_and_release
contains(QT_CONFIG, build_all):CONFIG += build_all

# workaround for QTBUG-31586
contains(QT_CONFIG, c++11): CONFIG += c++11

*-g++* {
    QMAKE_CXXFLAGS += -O3 -ftree-vectorize -ffast-math -funsafe-math-optimizations -Wno-error=return-type
}

SOURCES += common/shapes.cpp \
           sweep/sweep_context.cpp \
           sweep/cdt.cpp \
           sweep/sweep.cpp \
           sweep/advancing_front.cpp

# We don't need to install this lib, it's only used for building qtlocation
# However we do have to make sure that 'make install' builds it.
dummytarget.CONFIG = dummy_install
INSTALLS += dummytarget
