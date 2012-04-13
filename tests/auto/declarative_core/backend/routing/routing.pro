TEMPLATE = app
TARGET = tst_routing
# see QTBUG-25326 for why this is insignificant
CONFIG += warn_on qmltestcase insignificant_test
SOURCES += tst_routing.cpp

CONFIG(qtlocation-backend-tests) DEFINES += QTLOCATION_BACKEND_TESTS

QT += location quick testlib

OTHER_FILES = *.qml

TESTDATA = $$OTHER_FILES
