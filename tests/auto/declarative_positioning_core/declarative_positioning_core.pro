# QML tests in this directory must not depend on an OpenGL context.
# QML tests that do require an OpenGL context must go in ../../declarative_ui.

TEMPLATE = app
TARGET = tst_declarative_positioning_core
CONFIG += qmltestcase
SOURCES += main.cpp

CONFIG -= app_bundle

QT += positioning quick

OTHER_FILES = *.qml
TESTDATA = $$OTHER_FILES
