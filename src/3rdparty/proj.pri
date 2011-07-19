INCLUDEPATH += $$PWD/proj/include

DEFINES += DISABLE_CVSID

symbian {
  LIBS += -llibpthread -llibm
}
