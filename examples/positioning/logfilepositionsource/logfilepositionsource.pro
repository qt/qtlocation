TEMPLATE = app
TARGET = logfilepositionsource
QT = positioning core widgets


HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

EXAMPLE_FILES = simplelog.txt

target.path = $$[QT_INSTALL_EXAMPLES]/positioning/logfilepositionsource
INSTALLS += target
