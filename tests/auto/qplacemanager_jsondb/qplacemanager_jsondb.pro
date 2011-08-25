load(qttest_p4)

TARGET = tst_qplacemanager_jsondb

HEADERS += jsondbcleaner.h

SOURCES += tst_qplacemanager_jsondb.cpp \
           jsondbcleaner.cpp



QT += location testlib script
CONFIG += debug

DEFINES += QT_ADDON_JSONDB_LIB
QT += jsondb


