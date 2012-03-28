TEMPLATE = app
TARGET = tst_routing
CONFIG += warn_on qmltestcase
SOURCES += tst_routing.cpp

CONFIG(qtlocation-backend-tests) DEFINES += QTLOCATION_BACKEND_TESTS

QT += location quick testlib

TESTDATA += Fixture.qml
TESTDATA += tst_routing.qml
TESTDATA += tst_routing_no_route.qml
TESTDATA += tst_routing_features.qml
TESTDATA += tst_routing_travelmode.qml
TESTDATA += tst_routing_optimizations.qml
TESTDATA += tst_routing_alternative_routes.qml
TESTDATA += tst_routing_excluded_areas.qml
TESTDATA += tst_routing_localization.qml
TESTDATA += tst_unsupported_setups.qml
