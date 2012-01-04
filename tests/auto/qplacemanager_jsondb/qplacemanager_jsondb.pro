!no_system_tests:CONFIG += testcase
TARGET = tst_qplacemanager_jsondb

HEADERS += jsondbcleaner.h

SOURCES += tst_qplacemanager_jsondb.cpp \
           jsondbcleaner.cpp



QT += location testlib

DEFINES += QT_ADDON_JSONDB_LIB
QT += jsondb


