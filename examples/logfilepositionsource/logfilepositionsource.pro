TEMPLATE=app
INCLUDEPATH += ../../src/location

HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

CONFIG += console

include(../mobility_examples.pri)

CONFIG += mobility
MOBILITY = location

wince* {
    addFiles.sources = ./simplelog.txt
    addFiles.path = .
    DEPLOYMENT += addFiles
} else {
    logfileexample.path = $$QT_MOBILITY_EXAMPLES
    logfileexample.files = simplelog.txt
    INSTALLS += logfileexample
}

