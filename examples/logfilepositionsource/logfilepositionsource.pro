TEMPLATE = app
TARGET = logfilepositionsource
QT = location core widgets


HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/logfilepositionsource
sources.files = $$SOURCES $$HEADERS *.pro simplelog.txt
sources.path = $$[QT_INSTALL_EXAMPLES]/qtlocation/logfilepositionsource

INSTALLS += target sources
