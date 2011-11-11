TARGET = logfilepositionsource
TEMPLATE = app
QT = location core widgets


HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

wince* {
    addFiles.sources = ./simplelog.txt
    addFiles.path = .
    DEPLOYMENT += addFiles
} else {
    logfileexample.path = $$PWD
    logfileexample.files = simplelog.txt
    INSTALLS += logfileexample
}

