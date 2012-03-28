# QML tests in this directory must not depend on an OpenGL context.
# QML tests that do require an OpenGL context must go in ../../declarative_ui.

TEMPLATE = app
TARGET = tst_frontend
CONFIG += warn_on qmltestcase
SOURCES += main.cpp

QT += location quick

OTHER_FILES = *.qml
TESTDATA = $$OTHER_FILES

