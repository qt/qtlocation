# Generated from qtlocation.pro.

cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(QtLocation
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt Location Libraries"
    HOMEPAGE_URL "https://qt.io/"
    LANGUAGES CXX C
)

# Make sure we use the fixed BASE argument of qt_add_resource.
set(QT_USE_FIXED_QT_ADD_RESOURCE_BASE TRUE)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core) # special case
find_package(Qt6 ${PROJECT_VERSION} QUIET CONFIG OPTIONAL_COMPONENTS Quick Qml Gui Widgets Network Test DBus SerialPort QuickTest) # special case
# add simulator later. Probably smth else from QtLocation dependencies

if(WASM)
    message(NOTICE "Skipping the build as the condition \"NOT WASM\" is not met.")
    return()
endif()
qt_build_repo()
