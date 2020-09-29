TARGET = Qt$${QT_MAJOR_VERSION}AndroidPositioning

load(qt_build_paths)

CONFIG += java
DESTDIR = $$MODULE_BASE_OUTDIR/jar

JAVACLASSPATH += $$PWD/src

JAVASOURCES += \
        $$PWD/src/org/qtproject/qt/android/positioning/QtPositioning.java

# install
target.path = $$[QT_INSTALL_PREFIX]/jar
INSTALLS += target

