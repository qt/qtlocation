QT       += core network declarative

TARGET = qmlplaces

TEMPLATE = app

SOURCES += main.cpp

OTHER_FILES = qmlplaces.qml \
    Button.qml \
    CategoryDelegate.qml \
    SearchResultDelegate.qml \
    ReviewDelegate.qml \
    CategoryView.qml \
    ImageGrid.qml \
    EditorialDelegate.qml \
    RatingView.qml

RESOURCES += qmlplaces.qrc
