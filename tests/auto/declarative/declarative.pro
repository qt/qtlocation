TEMPLATE=app
TARGET=tst_qmlmapsandnavtest
CONFIG += warn_on qmltestcase
SOURCES += tst_qmlmapsandnav.cpp

QT += declarative location

OTHERFILES += *.qml

#qml.path = $$DESTDIR
#qml.files = *.qml
#INSTALLS += qml
