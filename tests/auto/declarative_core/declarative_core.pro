# QML tests in this directory must not depend on an OpenGL context.
# QML tests that do require an OpenGL context must go in ../declarative_ui.

TEMPLATE = app
TARGET = tst_declarative_core
CONFIG += warn_on qmltestcase
SOURCES += main.cpp

QT += declarative location

OTHER_FILES += *.qml

