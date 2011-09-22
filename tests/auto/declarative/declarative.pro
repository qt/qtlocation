TEMPLATE=app
TARGET=tst_qmlmapsandnavtest
CONFIG += warn_on qmltestcase
SOURCES += tst_qmlmapsandnav.cpp

QT += declarative location

OTHER_FILES += *.qml

#qml.path = $$DESTDIR
#qml.files = *.qml
#INSTALLS += qml

CONFIG += insignificant_test    # broken (refers to nonexistent types), QTBUG-21619
