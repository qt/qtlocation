TEMPLATE = lib
TARGET = poly2tri

CONFIG += staticlib
CONFIG -= qt

*-g++* {
    QMAKE_CXXFLAGS += -O3 -ftree-vectorize -ffast-math -funsafe-math-optimizations
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
