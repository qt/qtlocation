INCLUDEPATH += $$PWD/proj/include

DEFINES += DISABLE_CVSID

symbian {
  LIBS += -llibpthread -llibm
}

!contains(BUILD_PROJ, true) {
    LIBS += -L$$QT_BUILD_TREE/../qtlocation/src/3rdparty -lproj
}
