!no_system_tests:CONFIG += testcase
TARGET = tst_qplacemanager_jsondb

HEADERS += jsondbutils.h \
           ..\placemanager_utils\placemanager_utils.h

SOURCES += tst_qplacemanager_jsondb.cpp \
           jsondbutils.cpp \
           ..\placemanager_utils\placemanager_utils.cpp

QT += location testlib

QT += jsondb

DEFINES += JSONDB_DAEMON_BASE=\\\"$$QT.jsondb.bins\\\"

RESOURCES += resources.qrc

DEFINES += JSONFILE=\\\"$$PWD/../../../src/plugins/geoservices/nokia_places_jsondb/10-placesIndices.json\\\"
