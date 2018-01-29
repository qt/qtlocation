TARGET = QtLocationLabs

QT += core-private gui-private quick-private location-private positioning-private positioningquick-private

CONFIG += simd optimize_full

SOURCES += $$files(*.cpp)
HEADERS += $$files(*.h)

load(qt_module)

