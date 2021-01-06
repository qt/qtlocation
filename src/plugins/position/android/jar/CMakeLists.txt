# Generated from jar.pro.

set(java_sources
    src/org/qtproject/qt5/android/positioning/QtPositioning.java
)

qt_internal_add_jar(QtPositioning
    INCLUDE_JARS ${QT_ANDROID_JAR}
    SOURCES ${java_sources}
    OUTPUT_DIR "${QT_BUILD_DIR}/jar"
)

install_jar(QtPositioning
    DESTINATION jar
    COMPONENT Devel
)

