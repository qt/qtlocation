QT       += core network declarative

TARGET = qmlplaces

TEMPLATE = app

SOURCES += main.cpp

OTHER_FILES = qmlplaces.qml \
    Button.qml \
    CategoryDelegate.qml \
    SearchResultDelegate.qml \
    ReviewDelegate.qml

RESOURCES += qmlplaces.qrc

symbian {
    #Symbian specific definitions
    TARGET.UID3 = 0xE7E7BCD6
    TARGET.CAPABILITY = NetworkServices
    TARGET.EPOCALLOWDLLDATA = 1
}




