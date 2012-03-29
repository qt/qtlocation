!no_system_tests:CONFIG += testcase
TARGET = tst_qplacemanager_jsondb

HEADERS += jsondbutils.h

SOURCES += tst_qplacemanager_jsondb.cpp \
           jsondbutils.cpp



QT += location testlib

QT += jsondb

DEFINES += JSONDB_DAEMON_BASE=\\\"$$QT.jsondb.bins\\\"

RESOURCES += resources.qrc
