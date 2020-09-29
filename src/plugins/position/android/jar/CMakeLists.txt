# Generated from jar.pro.

set(java_sources
    src/org/qtproject/qt/android/positioning/QtPositioning.java # special case
)

qt_internal_add_jar(Qt${QtLocation_VERSION_MAJOR}AndroidPositioning # special case
    INCLUDE_JARS ${QT_ANDROID_JAR}
    SOURCES ${java_sources}
    OUTPUT_DIR "${QT_BUILD_DIR}/jar"
)

install_jar(Qt${QtLocation_VERSION_MAJOR}AndroidPositioning # special case
    DESTINATION jar
    COMPONENT Devel
)

