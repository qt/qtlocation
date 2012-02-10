!no_system_tests:CONFIG += testcase
TARGET = tst_qplacemanager_jsondb

HEADERS += jsondbcleaner.h

SOURCES += tst_qplacemanager_jsondb.cpp \
           jsondbcleaner.cpp



QT += location testlib

QT += jsondbcompat

RESOURCES += resources.qrc
